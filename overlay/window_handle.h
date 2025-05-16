#pragma once

#include <Windows.h>

LRESULT WINAPI WndProc_custom(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL use_uiaccess(HWND* pHandle);
BOOL hook_nvidia(HWND* pHandle);
BOOL new_window(HWND* pHandle);


constexpr auto getWindowHandle = new_window;