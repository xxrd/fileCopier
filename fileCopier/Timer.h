#pragma once
#include <chrono>

using namespace std;

// Класс для замера времени
class Timer {
private:
	using clock_t = chrono::high_resolution_clock;
	using second_t = chrono::duration<double, std::ratio<1> >;
	chrono::time_point<clock_t> m_beg;
public:
	Timer();

	// Сбрасывает время таймера на текущее
	void reset();

	// Вовзращает прошедшее время со старта таймера
	double elapsed() const;
};