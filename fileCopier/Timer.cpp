#include "Timer.h"

// Класс для замера времени
Timer::Timer() : m_beg(clock_t::now()) {}

// Сбрасывает время таймера на текущее
void Timer::reset() {
	m_beg = clock_t::now();
}

// Вовзращает прошедшее время со старта таймера
double Timer::elapsed() const {
	return chrono::duration_cast<second_t>(clock_t::now() - m_beg).count();
}
