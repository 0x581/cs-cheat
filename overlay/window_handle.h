#pragma once

#include <Windows.h>


BOOL use_uiaccess(HWND* pHandle);
BOOL hook_nvidia(HWND* pHandle);
BOOL new_window(HWND* pHandle);


constexpr auto getWindowHandle = new_window;