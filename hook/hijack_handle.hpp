#pragma once

#include <Windows.h>

DWORD getPID(const wchar_t* processName);

HANDLE HijackExistingHandle(DWORD dwTargetProcessId, const wchar_t* fromProcess);

HANDLE HijackExistingHandle(const wchar_t* targetProcess, const wchar_t* fromProcess);
