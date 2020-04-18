#pragma once
#include <initializer_list>
#include <unordered_map>
#include <mutex>

using namespace std;

// Соответствие символов команде
struct Commands {
	static const char stopCopying = 'Q';
	static const char continueCopying = 'R';
};

// Структура для работы с командами, которую будут разделять потоки
class CommandFlags {
private:
	mutex commandsFlagsMutex;
	unordered_map<char, bool> commandFlags;
public:
	CommandFlags(initializer_list<char> list);
	void addCommand(char ch, bool value = false);
	void changeCommandFlag(char ch, bool value);
	bool getCommandFlag(char ch);
};