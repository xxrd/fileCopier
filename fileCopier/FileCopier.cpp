#include "FileCopier.h"
#include <Windows.h>
#include <string>
#include "util.h"
#include <iostream>
#include <conio.h>
#include <thread>
#include "Timer.h"
#include <sstream>

// Генерирует имя для файла-копии, добавляет постфикс "- copy" при необходимости
filesystem::path FileCopier::generateNameForOutputFile(const filesystem::path& path, const filesystem::path& filename) const {
	filesystem::path copyFilename;
	if (filesystem::exists(path / filename)) {
		string filenameStr = filename.string();
		int n = filenameStr.find_last_of(".");
		copyFilename = filenameStr.substr(0, n) + " - copy" + filenameStr.substr(n);
	}
	else {
		return filename;
	}
	return generateNameForOutputFile(path, copyFilename);
}

filesystem::path FileCopier::generateNameForOutputFile() const {
	return generateNameForOutputFile(pathTo, pathFrom.filename());
}

// Возвращает объем доступного свободного пространства на диске, указанном в path
long long FileCopier::freeSpaceOnDisk(const filesystem::path& path) const {
	BOOL  fResult;
	unsigned __int64 i64FreeBytesToCaller,
		i64TotalBytes,
		i64FreeBytes;
	fResult = GetDiskFreeSpaceEx(path.root_name().c_str(),
		(PULARGE_INTEGER)&i64FreeBytesToCaller,
		(PULARGE_INTEGER)&i64TotalBytes,
		(PULARGE_INTEGER)&i64FreeBytes);
	if (fResult)
	{
		return i64FreeBytesToCaller;
	}
	return -1;
}

// Закрывает потоки ввода/вывода и удаляет созданный выходной файл
void FileCopier::stopCopying() {
	in.close();
	out.close();
	filesystem::remove(outputFilePath);
}

// Выводит в консоль прогресс копирования, скорость и время до окончания копирования
void FileCopier::showProgress() const {
	clear();
	stringstream s;
	s << fixed << setprecision(2) <<
		"Copy progress: " << convertSize(copiedBytesCount, 1024 * 1024) << "MB("
		<< copyProgress << "%) of " << convertSize(inputFileSize, 1024 * 1024) << "MB(100%)\n"
		<< "data rate: " << convertSize(dataRate, 1024 * 1024) << " MB/S, "
		<< "approximate time to finish: " << convertSize(getTimeToStopCopying(), 60) << " minutes\n"
		<< "\n(press 'Q' to stop copying)\n";
	cout << s.str();
}

// Очищает консоль и выводит строку
void FileCopier::clearAndShow(string str) const {
	clear();
	cout << str;
}

// Слушает поток ввода и изменяет флаги в разделяемой потоками структуре, чтобы оповестить другой
// поток о нажатии на кнопку команды 
void FileCopier::listenCommand(CommandFlags& cm) {
	while (true) {
		switch (toupper(_getch())) {
		case 'Q':
			cm.changeCommandFlag('Q', true);
			break;
		case 'R':
			cm.changeCommandFlag('R', true);
			break;
		}
	}
}

// Обновляет таймеры и скорость передачи
void FileCopier::updateDataRate() {
	const int oneSecond = 1;
	const int threeSeconds = 3;

	if (timer.elapsed() >= threeSeconds) {
		timer.reset();
	}
	else if (timer.elapsed() >= oneSecond) {
		dataRate = copiedBytesCount / ((int)timerFromStartCopying.elapsed());
	}
}

// Возвращает время до окончания копирования
double FileCopier::getTimeToStopCopying() const {
	return (inputFileSize - copiedBytesCount) / dataRate;
}

FileCopier::FileCopier(filesystem::path pathFrom, filesystem::path pathTo)
	: pathFrom(pathFrom), pathTo(pathTo), bufSize(BUFSIZ) {
	inputFileSize = filesystem::file_size(pathFrom);
	buf = shared_ptr<char[]>(new char[bufSize]);
	onePercentOfInputFile = inputFileSize / 100;
	copyProgress = 0;
	copiedBytesCount = 0;
}

void FileCopier::run() {
	in = ifstream(pathFrom, ios::in | ios::binary);
	if (!in) {
		throw logic_error("File: " + pathFrom.string() + " cannot be opened for reading\n");
	}

	outputFilePath = pathTo / generateNameForOutputFile(pathTo, pathFrom.filename());
	out = ofstream(outputFilePath, ios::out | ios::binary);
	if (!out) {
		throw logic_error("File: " + outputFilePath.string() + " cannot be opened for writing\n");
	}

	// Если файл пустой, то копирование уже завершено
	if (inputFileSize == 0) {
		cout << "File copied successfully\n";
		return;
	}

	// Проверка, хватает ли памяти на диске
	long long freeSpace = freeSpaceOnDisk(pathTo);
	if (freeSpace < inputFileSize) {
		string errStr("");
		errStr += "Not enough disk space to copy\n";
		errStr += "File size: " + to_string(convertSize(inputFileSize, 1024 * 1024)) + "MB\n";
		errStr += "Free disk space: " + to_string(convertSize(freeSpace, 1024 * 1024)) + "MB\n";
		errStr += "Need: " + to_string(convertSize(inputFileSize - freeSpace, 1024 * 1024)) + "MB\n";
		stopCopying();
		throw runtime_error(errStr);
	}
	else if (freeSpace == -1) {
		stopCopying();
		throw runtime_error("Error occurred while checking disk and file sizes\n");
	}

	// Поток, слушающий ввод команд
	CommandFlags commandFlags({ Commands::stopCopying, Commands::continueCopying });
	thread th(&FileCopier::listenCommand, this, ref(commandFlags));
	th.detach();

	showProgress();

	// Копирование
	timerFromStartCopying.reset();
	timer.reset();
	while (in) {
		
		// Обновляет скорость передачи
		updateDataRate();

		// Если памяти в процессе копирования перестанет хватать, то будет предложение либо выйти, либо освободить память
		// и продолжить копирование
		if (!out) {
			uintmax_t leftToCopy = inputFileSize - copiedBytesCount;
			uintmax_t outputDiskSpace = freeSpaceOnDisk(outputFilePath);

			if (outputDiskSpace < leftToCopy) {
				clear();
				cout << "No memory on disk\n";
				cout << "Disk space remaining: " << convertSize(outputDiskSpace, 1024 * 1024) << "MB\n";
				cout << "Copied: " << convertSize(copiedBytesCount, 1024 * 1024) << "MB of " << convertSize(inputFileSize, 1024 * 1024) << "MB\n";
				cout << "Need to free up memory: " << convertSize(leftToCopy - outputDiskSpace, 1024 * 1024) << "MB\n";
				cout << "Free up disk space and press 'R' to continue or press 'Q' if you want to stop copying";

				commandFlags.changeCommandFlag(Commands::continueCopying, false);
				while (true) {
					if (commandFlags.getCommandFlag(Commands::stopCopying) == true) {
						stopCopying();
						clearAndShow("Copy canceled\n");
						return;
					}
					if (commandFlags.getCommandFlag(Commands::continueCopying) == true) {
						break;
					}
				}
			}
			out.clear();
			showProgress();
			continue;
		}

		// Если нажата клавиша Q, то уже скопированные данные удаляются
		if (commandFlags.getCommandFlag(Commands::stopCopying)) {
			stopCopying();
			clearAndShow("Copy canceled\n");
			return;
		}

		in.read(buf.get(), bufSize);
		out.write(buf.get(), in.gcount());

		// Вывод прогресса копирования
		copiedBytesCount += in.gcount();
		unsigned copyPercentage = copiedBytesCount / onePercentOfInputFile;
		if (copyPercentage > copyProgress) {
			copyProgress = copyPercentage;
			showProgress();
		}

	}
	clearAndShow("File copied successfully\n");
}