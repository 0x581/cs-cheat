#pragma once

#include <Windows.h>

DWORD getPID(const wchar_t* processName);

HANDLE hijackHandle(DWORD dwTargetProcessId, const wchar_t* fromProcess);

HANDLE hijackHandle(const wchar_t* targetProcess, const wchar_t* fromProcess);
