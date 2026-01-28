#pragma once

#include <Windows.h>

BOOL use_uiaccess(HWND *pHandle);
BOOL hook_nvidia(HWND *pHandle);
BOOL new_window(HWND *pHandle);

BOOL update_wnd_proc(HWND handle);

#ifdef USE_NVIDIA_HOOK
constexpr auto getWindowHandle = hook_nvidia;
#elifdef USE_NEW_WINDOW
constexpr auto getWindowHandle = new_window;
#else
constexpr auto getWindowHandle = use_uiaccess;
#endif
