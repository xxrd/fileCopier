#pragma once
#include <chrono>

using namespace std;

// ����� ��� ������ �������
class Timer {
private:
	using clock_t = chrono::high_resolution_clock;
	using second_t = chrono::duration<double, std::ratio<1> >;
	chrono::time_point<clock_t> m_beg;
public:
	Timer();

	// ���������� ����� ������� �� �������
	void reset();

	// ���������� ��������� ����� �� ������ �������
	double elapsed() const;
};