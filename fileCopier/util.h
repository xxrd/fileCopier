#pragma once

// Очищает консоль
void clear();

// Для преобразований единиц измерения
template<class T, class U>
auto convertSize(T n, U d) -> decltype(n / d) {
	if (n == 0) return 0;
	else return n / d;
}
