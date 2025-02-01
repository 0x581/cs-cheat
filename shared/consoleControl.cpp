#pragma once
#include <string>
#include <Windows.h>

#include "sharedVariables.hpp"

inline static COORD getTerminalSize()
{
	CONSOLE_SCREEN_BUFFER_INFO sBufferInfo{ 0 };
	GetConsoleScreenBufferInfo(consoleOutput, &sBufferInfo);

	return sBufferInfo.dwSize;
}

inline static COORD setCursorPosition(short x, short y) {
	CONSOLE_SCREEN_BUFFER_INFO sBufferInfo{ 0 };
	GetConsoleScreenBufferInfo(consoleOutput, &sBufferInfo);
	SetConsoleCursorPosition(consoleOutput, COORD{ .X = x, .Y = y });

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

#define printFPS do {																										\
	static auto counter = 0;																								\
	static auto last = std::chrono::high_resolution_clock::now();															\
	if (++counter % 8 == 0)	{																								\
		const auto fps = std::to_string(std::chrono::seconds(1) / (std::chrono::high_resolution_clock::now() - last));		\
		const auto terminalSize = getTerminalSize();																		\
		const auto prev = setCursorPosition(terminalSize.X - (fps.length() + 4), 0);										\
		std::cout << fps << " fps";																							\
		setCursorPosition(prev.X, prev.Y);																					\
	}																														\
	last = std::chrono::high_resolution_clock::now();																		\
} while (false)
