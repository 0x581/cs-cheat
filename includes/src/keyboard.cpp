#include "../keyboard.h"

#include <Windows.h>
#include <bitset>

static std::bitset<256> g_keyStates;
static int mostRecentKey = -1;

static HHOOK g_keyboardHook = nullptr;

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

const bool IsKeyPressed(int keyCode)
{
	if (keyCode < 0 || keyCode >= 256)
		return false;

	return g_keyStates[keyCode];
}

bool init_keyboard_hook()
{
	g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);

	return g_keyboardHook != NULL;
}
