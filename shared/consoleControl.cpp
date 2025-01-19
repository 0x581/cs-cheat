#pragma once
#include <string>
#include <Windows.h>

#include "sharedVariables.hpp"

inline static std::string getPositioningString(short x, short y) {
    SetConsoleCursorPosition(consoleOutput, COORD{.X = x, .Y = y});

	//print(f"\033[{y};{x}H", end = "")
}

inline static void hideCursor() {
	static const CONSOLE_CURSOR_INFO info{ .dwSize = 100, .bVisible = false };
	SetConsoleCursorInfo(consoleOutput, &info);
}

inline static void showCursor() {
	static const CONSOLE_CURSOR_INFO info{ .dwSize = 100, .bVisible = true };
	SetConsoleCursorInfo(consoleOutput, &info);
}

inline static const HANDLE getConsoleOutputHandle() { return GetStdHandle(STD_OUTPUT_HANDLE); }
inline static const HANDLE getConsoleInputHandle() { return GetStdHandle(STD_INPUT_HANDLE); }
inline static const HANDLE getConsoleErrorHandle() { return GetStdHandle(STD_ERROR_HANDLE); }
