#pragma once
#define _HAS_STD_BYTE 0 // фикс конфликта типа byte из std и windows.h, чтобы можно было использовать using namespace std
#include <filesystem>
#include <fstream>
#include "CommandFlags.h"
#include "Timer.h"

using namespace std;

class FileCopier {
private:
	filesystem::path pathFrom; // Путь к файлу(включает имя и расширение файла)
	filesystem::path pathTo; // Путь в директорию, куда копировать
	filesystem::path outputFilePath; // путь в выходной файл, включающий его имя и расширение
	ifstream in; // Поток в входной файл
	ofstream out; // Поток в выходной файл
	uintmax_t inputFileSize; // Размер копируемого файла
	const unsigned bufSize; // Размер буфера для копирования(BUFSIZ)
	shared_ptr<char[]> buf; // Буфер для копирования
	unsigned copyProgress; // Прогресс копирования
	uintmax_t copiedBytesCount; // Количество скопированных байт
	long double onePercentOfInputFile; // Процент от размера входного файла
	Timer timerFromStartCopying; // Таймер, запускаемый при начале копирования
	Timer timer; // Таймер для определения скорости передачи
	double dataRate; // Скорость передачи данных

	// Генерирует имя для файла-копии, добавляет постфикс "- copy" при необходимости
	filesystem::path generateNameForOutputFile(const filesystem::path& path, const filesystem::path& filename) const;

	filesystem::path generateNameForOutputFile() const;

	// Возвращает объем доступного свободного пространства на диске, указанном в path
	long long freeSpaceOnDisk(const filesystem::path& path) const;

	// Закрывает потоки ввода/вывода и удаляет созданный выходной файл
	void stopCopying();

	// Выводит в консоль прогресс копирования, если он изменен
	void showProgress() const;

	// Очищает консоль и выводит строку
	void clearAndShow(string str) const;

	// Слушает поток ввода и изменяет флаги в разделяемой потоками структуре, чтобы оповестить другой
	// поток о нажатии на кнопку команды 
	void listenCommand(CommandFlags& cm);

	// Обновляет таймеры и скорость передачи
	void updateDataRate();

	// Возвращает время до окончания копирования
	double getTimeToStopCopying() const;

public:
	FileCopier(filesystem::path pathFrom, filesystem::path pathTo);
	void run();
};