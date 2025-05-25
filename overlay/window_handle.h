#pragma once

#include <Windows.h>

BOOL use_uiaccess(HWND* pHandle);
BOOL hook_nvidia(HWND* pHandle);
BOOL new_window(HWND* pHandle);

BOOL update_wnd_proc(HWND handle);

constexpr auto getWindowHandle = hook_nvidia;
