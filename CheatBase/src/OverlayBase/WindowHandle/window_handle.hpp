#pragma once

#include <Windows.h>
#include <iostream>

BOOL use_uiaccess(HWND *pHandle);
BOOL hook_nvidia(HWND *pHandle);
BOOL new_window(HWND *pHandle);

BOOL update_wnd_proc(HWND handle);

// Try UIAccess first (required for overlay on top of elevated windows),
// fall back to NVIDIA hook (works when GeForce Overlay is running),
// fall back to a plain topmost window (works with CS2 borderless windowed).
inline BOOL getWindowHandle(HWND *pHandle)
{
#ifndef USE_NEW_WINDOW
#ifndef USE_NVIDIA_HOOK
    if (use_uiaccess(pHandle)) {
        std::cout << "Window: UIAccess" << std::endl;
        return TRUE;
    }
    std::cout << "UIAccess failed, trying NVIDIA hook..." << std::endl;
    if (hook_nvidia(pHandle)) {
        std::cout << "Window: NVIDIA hook" << std::endl;
        return TRUE;
    }
    std::cout << "NVIDIA hook failed, using plain window..." << std::endl;
#else
    if (hook_nvidia(pHandle)) {
        std::cout << "Window: NVIDIA hook" << std::endl;
        return TRUE;
    }
    std::cout << "NVIDIA hook failed, using plain window..." << std::endl;
#endif
#endif
    if (new_window(pHandle)) {
        std::cout << "Window: plain topmost" << std::endl;
        return TRUE;
    }
    return FALSE;
}
