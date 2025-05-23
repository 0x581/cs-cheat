#include "../keyboard.h"

#include <Windows.h>
#include <bitset>

std::bitset<256> g_keyStates = { false };
static int mostRecentKey = -1;

static HHOOK g_keyboardHook = nullptr;
static HHOOK g_mouseHook = nullptr;

static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		KBDLLHOOKSTRUCT* pKbdStruct = (KBDLLHOOKSTRUCT*)lParam;

		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
		{
			g_keyStates[pKbdStruct->vkCode] = true;
			mostRecentKey = pKbdStruct->vkCode;
		}
		else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
			g_keyStates[pKbdStruct->vkCode] = false;
	}
	return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

static LRESULT CALLBACK LowLevelMouseProc(
	int nCode,
	WPARAM wParam,
	LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		MSLLHOOKSTRUCT* pMhs = (MSLLHOOKSTRUCT*)lParam;
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
				key = HIWORD(pMhs->mouseData);
				down = true;
				break;
			case WM_XBUTTONUP:
				key = HIWORD(pMhs->mouseData);
				down = false;
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

bool init_keyboard_hook()
{
	g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);

	g_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(NULL), 0);

	return (g_keyboardHook != NULL) && (g_mouseHook != NULL);
}

void clear_keyboard_hook()
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
