#pragma once
#define _HAS_STD_BYTE 0 // ���� ��������� ���� byte �� std � windows.h, ����� ����� ���� ������������ using namespace std
#include <filesystem>
#include <fstream>
#include "CommandFlags.h"

using namespace std;

class FileCopier {
private:
	filesystem::path pathFrom; // ���� � �����(�������� ��� � ���������� �����)
	filesystem::path pathTo; // ���� � ����������, ���� ����������
	filesystem::path outputFilePath; // ���� � �������� ����, ���������� ��� ��� � ����������
	ifstream in; // ����� � ������� ����
	ofstream out; // ����� � �������� ����
	uintmax_t inputFileSize; // ������ ����������� �����
	const unsigned bufSize; // ������ ������ ��� �����������(BUFSIZ)
	shared_ptr<char[]> buf; // ����� ��� �����������
	unsigned copyProgress; // �������� �����������
	uintmax_t copiedBytesCount; // ���������� ������������� ����
	long double onePercentOfInputFile; // ������� �� ������� �������� �����


	// ���������� ��� ��� �����-�����, ��������� �������� "- copy" ��� �������������
	filesystem::path generateNameForOutputFile(const filesystem::path& path, const filesystem::path& filename) const;

	filesystem::path generateNameForOutputFile() const;

	// ���������� ����� ���������� ���������� ������������ �� �����, ��������� � path
	long long freeSpaceOnDisk(const filesystem::path& path) const;

	// ��������� ������ �����/������ � ������� ��������� �������� ����
	void stopCopying();

	// ������� � ������� �������� �����������, ���� �� �������
	void showProgress();

	// ������� ������� � ������� ������
	void clearAndShow(string str);

	// ������� ����� ����� � �������� ����� � ����������� �������� ���������, ����� ���������� ������
	// ����� � ������� �� ������ ������� 
	void listenCommand(CommandFlags& cm);

public:
	FileCopier(filesystem::path pathFrom, filesystem::path pathTo);
	void run();
};