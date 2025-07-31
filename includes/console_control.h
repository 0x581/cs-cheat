#pragma once

#include <Windows.h>

inline static COORD getTerminalSize();
inline static COORD setCursorPosition(short x, short y);

inline static void hideCursor();
inline static void showCursor();

inline static const HANDLE getConsoleOutputHandle() { return GetStdHandle(STD_OUTPUT_HANDLE); }
inline static const HANDLE getConsoleInputHandle() { return GetStdHandle(STD_INPUT_HANDLE); }
inline static const HANDLE getConsoleErrorHandle() { return GetStdHandle(STD_ERROR_HANDLE); }

#define printFPS                                                                                                           \
    do                                                                                                                     \
    {                                                                                                                      \
        static auto counter = 0;                                                                                           \
        static auto last = std::chrono::high_resolution_clock::now();                                                      \
        if (++counter % 8 == 0)                                                                                            \
        {                                                                                                                  \
            const auto fps = std::to_string(std::chrono::seconds(1) / (std::chrono::high_resolution_clock::now() - last)); \
            const auto terminalSize = getTerminalSize();                                                                   \
            const auto prev = setCursorPosition((short)(terminalSize.X - (fps.length() + 4)), 0);                          \
            std::cout << fps << " fps";                                                                                    \
            setCursorPosition((short)prev.X, (short)prev.Y);                                                               \
        }                                                                                                                  \
        last = std::chrono::high_resolution_clock::now();                                                                  \
    } while (false)
