#include <Windows.h>

#include "../shared_variables.h"
#include "../console_control.h"

inline static COORD getTerminalSize()
{
    CONSOLE_SCREEN_BUFFER_INFO sBufferInfo{0};
    GetConsoleScreenBufferInfo(consoleOutput, &sBufferInfo);

    return sBufferInfo.dwSize;
}

inline static COORD setCursorPosition(short x, short y)
{
    CONSOLE_SCREEN_BUFFER_INFO sBufferInfo{0};
    GetConsoleScreenBufferInfo(consoleOutput, &sBufferInfo);
    SetConsoleCursorPosition(consoleOutput, COORD{.X = x, .Y = y});

    return sBufferInfo.dwCursorPosition;
}

inline static void hideCursor()
{
    static const CONSOLE_CURSOR_INFO info{.dwSize = 100, .bVisible = false};
    SetConsoleCursorInfo(consoleOutput, &info);
}

inline static void showCursor()
{
    static const CONSOLE_CURSOR_INFO info{.dwSize = 100, .bVisible = true};
    SetConsoleCursorInfo(consoleOutput, &info);
}
