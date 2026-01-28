#include "../keyboard.hpp"

#include <Windows.h>
#include <bitset>
#include <iostream>
#include <thread>

std::bitset<256> g_keyStates = {false};
int mostRecentKey = -1;

static HHOOK g_keyboardHook = nullptr;
static HHOOK g_mouseHook = nullptr;

static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    KBDLLHOOKSTRUCT *pKbdStruct = (KBDLLHOOKSTRUCT *)lParam;

    if (nCode != HC_ACTION || pKbdStruct == nullptr || pKbdStruct->vkCode >= 256)
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);

    const DWORD originalVkCode = pKbdStruct->vkCode;
    DWORD effectiveVkCode = originalVkCode;
    const bool isExtended = (pKbdStruct->flags & LLKHF_EXTENDED) != 0;
    const bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
    const bool isKeyUp = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

    if (originalVkCode == VK_RETURN && isExtended)
        effectiveVkCode = VK_SEPARATOR;

    if (isKeyDown)
    {
        if (!g_keyStates[effectiveVkCode])
        {
            g_keyStates[effectiveVkCode] = true;
            mostRecentKey = pKbdStruct->vkCode;
        }
    }
    else if (isKeyUp)
        g_keyStates[effectiveVkCode] = false;

    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        MSLLHOOKSTRUCT *pMhs = (MSLLHOOKSTRUCT *)lParam;
        if (pMhs != nullptr)
        {
            int key = -1;
            bool down = false;

            switch (wParam)
            {
            case WM_LBUTTONDOWN:
                key = VK_LBUTTON;
                down = true;
                break;
            case WM_LBUTTONUP:
                key = VK_LBUTTON;
                down = false;
                break;
            case WM_RBUTTONDOWN:
                key = VK_RBUTTON;
                down = true;
                break;
            case WM_RBUTTONUP:
                key = VK_RBUTTON;
                down = false;
                break;
            case WM_MBUTTONDOWN:
                key = VK_MBUTTON;
                down = true;
                break;
            case WM_MBUTTONUP:
                key = VK_MBUTTON;
                down = false;
                break;
            case WM_XBUTTONDOWN:
            case WM_XBUTTONUP:
                WORD xButtonFlags = HIWORD(pMhs->mouseData);

                if (xButtonFlags == XBUTTON1)
                    key = VK_XBUTTON1;
                else if (xButtonFlags == XBUTTON2)
                    key = VK_XBUTTON2;

                down = (wParam == WM_XBUTTONDOWN);
                break;
            }

            if (key >= 0 && key <= g_keyStates.size())
            {
                g_keyStates[key] = down;

                if (down)
                    mostRecentKey = key;
            }
        }
    }
    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

const bool IsKeyPressed(const int vkCode)
{
    if (vkCode < 0 || vkCode >= 256)
        return false;

    return g_keyStates[vkCode];
}

static bool init_input_hook()
{
    g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);

    if (!g_keyboardHook)
    {
        const auto error = GetLastError();
        std::cout << "Failed to create keyboard hook. Error code: " << error << std::endl;
    }

    g_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(NULL), 0);

    if (!g_mouseHook)
    {
        const auto error = GetLastError();
        std::cout << "Failed to create mouse hook. Error code: " << error << std::endl;
    }

    return (g_keyboardHook != NULL) && (g_mouseHook != NULL);
}

static void clear_input_hook()
{
    if (g_keyboardHook != NULL)
    {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = NULL;
    }

    if (g_mouseHook != NULL)
    {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = NULL;
    }
}

void run_input_hook(bool &terminate)
{
    const DWORD threadId = GetCurrentThreadId();

    if (!init_input_hook())
        throw std::runtime_error("Failed to create keyboard hook.");

    MSG msg;

    while (!terminate)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    clear_input_hook();

    std::cout << "Input hook terminated." << std::endl;
}
