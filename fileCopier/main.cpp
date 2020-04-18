#include <iostream>
#include <string>
#include "util.h"
#include "FileCopier.h"

using namespace std;

// путь к тому, что копировать - абсолютный путь в директорию копирования с именем файла, например C:\folder\file1.txt
// путь куда копировать - абсолютный путь в директорию копирования, например C:\folder

// при копировании отображается прогресc, отображается примерная скорость копирования и примерное время до окончания копирования
// для отмены копирования нужно нажать 'Q'

// если памяти на диске изначально не хватает, то копирования не будет,
// если стало не хватать в процессе, то копирование приостанавливается и предлагается освободить
// пространство и повторить попытку копирования c помощью нажатия клавиши 'R', либо отменить копирование нажав 'Q'

int main() {
	string pathString;

	cout << "Enter the path to the file you want to copy:" << endl;
	getline(cin, pathString);
	filesystem::path pathFrom(pathString);

	cout << "Enter the path to copy:" << endl;
	getline(cin, pathString);
	filesystem::path pathTo(pathString);
	clear();

	try {
		FileCopier app(pathFrom, pathTo);
		app.run();
	}
	catch (const exception& e) {
		cerr << e.what() << "\n";
		return 1;
	}
}