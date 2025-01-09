#pragma once

#include <Windows.h>
#include <iostream>

#include <TlHelp32.h>

class Memory {

private:
	DWORD id = 0; // PID
	HANDLE process = NULL; // Process handle

public:
	Memory(const char* processName) {
		PROCESSENTRY32W entry{};
		entry.dwSize = sizeof(PROCESSENTRY32W);

		const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		char currentProcess[MAX_PATH + 1]{ 0 };

		while (Process32Next(snapShot, &entry)) {

			if (wcstombs_s(nullptr, currentProcess, entry.szExeFile, MAX_PATH) == -1)
				continue;

			currentProcess[MAX_PATH] = 0;

			if (!strcmp(processName, currentProcess)) {
				this->id = entry.th32ProcessID;
				this->process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->id);
				break;
			}
		}

		if (snapShot)
			CloseHandle(snapShot);
	}

	~Memory() {
		if (this->process)
			CloseHandle(this->process);
	}

	DWORD getProcessId() const { return this->id; }
	HANDLE getProcessHandle() const { return this->process; }

	uintptr_t GetModuleAddress(const char* moduleName) const {
		MODULEENTRY32W entry{};
		entry.dwSize = sizeof(MODULEENTRY32W);

		const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, this->id);

		if ((size_t)snapShot == -1) {
			DWORD error = GetLastError();
			std::cout << "Snapshot failed: " << error << std::endl;
			return 0;
		}

		uintptr_t result = 0;

		char currentModule[MAX_MODULE_NAME32 + 1]{ 0 };

		while (Module32Next(snapShot, &entry)) {
			if (wcstombs_s(nullptr, currentModule, entry.szModule, MAX_MODULE_NAME32) == -1)
				continue;

			currentModule[MAX_MODULE_NAME32] = 0;

			if (!strcmp(moduleName, currentModule)) {
				result = reinterpret_cast<uintptr_t>(entry.modBaseAddr);
				break;
			}
		}

		if (snapShot)
			CloseHandle(snapShot);

		return result;
	}

	template <typename T>
	T Read(uintptr_t address) const  {
		T value;
		ReadProcessMemory(this->process, (LPCVOID)address, &value, sizeof(T), NULL);
		return value;
	}

	template <typename T>
	bool Write(uintptr_t address, T value) const {
		return WriteProcessMemory(this->process, (LPVOID)address, &value, sizeof(T), NULL);
	}
};
