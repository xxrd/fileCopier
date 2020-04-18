#include <iostream>
#include <string>
#include "util.h"
#include "FileCopier.h"

using namespace std;

// ���� � ����, ��� ���������� - ���������� ���� � ���������� ����������� � ������ �����, �������� C:\folder\file1.txt
// ���� ���� ���������� - ���������� ���� � ���������� �����������, �������� C:\folder
// ��� ����������� ������������ ��������, ��� ������ ����������� ����� ������ 'Q'

// ���� ������ �� ����� ���������� �� �������, �� ����������� �� �����,
// ���� ����� �� ������� � ��������, �� ����������� ������������������ � ������������ ����������
// ������������ � ��������� ������� ����������� c ������� ������� ������� 'R', ���� �������� ����������� ����� 'Q'

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