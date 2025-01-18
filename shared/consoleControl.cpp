#pragma once
#include <string>
#include <Windows.h>

#include "sharedVariables.hpp"

inline static std::string getPositioningString(short x, short y) {
    SetConsoleCursorPosition(console, COORD{.X = x, .Y = y});

	//print(f"\033[{y};{x}H", end = "")
}

inline static void hideCursor() {
	static const CONSOLE_CURSOR_INFO info{ .dwSize = 100, .bVisible = false };
	SetConsoleCursorInfo(console, &info);
}

inline static void showCursor() {
	static const CONSOLE_CURSOR_INFO info{ .dwSize = 100, .bVisible = true };
	SetConsoleCursorInfo(console, &info);
}
