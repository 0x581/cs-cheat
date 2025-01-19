#pragma once
#include <string>
#include <Windows.h>

#include "sharedVariables.hpp"

inline static COORD setCursorPosition(short x, short y) {
	CONSOLE_SCREEN_BUFFER_INFO sBufferInfo{ 0 };
	GetConsoleScreenBufferInfo(consoleOutput, &sBufferInfo);
    SetConsoleCursorPosition(consoleOutput, COORD{.X = x, .Y = y});

	return sBufferInfo.dwCursorPosition;
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
