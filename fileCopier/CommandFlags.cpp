#include "CommandFlags.h"
#include <ctype.h>

// Структура для работы с командами, которую будут разделять потоки
CommandFlags::CommandFlags(initializer_list<char> list) {
	for (char ch : list) {
		commandFlags[toupper(ch)] = false;
	}
}

void CommandFlags::addCommand(char ch, bool value) {
	lock_guard<mutex> lg(commandsFlagsMutex);
	commandFlags[toupper(ch)] = value;
}

void CommandFlags::changeCommandFlag(char ch, bool value) {
	lock_guard<mutex> lg(commandsFlagsMutex);
	commandFlags[toupper(ch)] = value;
}

bool CommandFlags::getCommandFlag(char ch) {
	lock_guard<mutex> lg(commandsFlagsMutex);
	try {
		return commandFlags.at(toupper(ch));
	}
	catch (const out_of_range&) {
		addCommand(toupper(ch), false);
		return false;
	}
}