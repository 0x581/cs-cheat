#pragma once

#include <Windows.h>

DWORD GetPID(const wchar_t* processName);

HANDLE HijackExistingHandle(DWORD dwTargetProcessId);
