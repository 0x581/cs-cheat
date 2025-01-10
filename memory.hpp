#pragma once

#include <Windows.h>
#include <iostream>
#include <sstream>
#include <iomanip>

#include <TlHelp32.h>

constexpr auto BYTES_PER_ROW = 32;

class Memory {

private:
	DWORD id = 0; // PID
	HANDLE process = NULL; // Process handle

public:
	Memory(const wchar_t* processName) {
		PROCESSENTRY32W entry{};
		entry.dwSize = sizeof(PROCESSENTRY32W);

		const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if (snapShot == INVALID_HANDLE_VALUE) {
			DWORD error = GetLastError();
			std::cout << "Process snapshot failed: " << error << std::endl;
			return;
		}

		while (Process32NextW(snapShot, &entry)) {
			if (!wcscmp(processName, entry.szExeFile)) {
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

	uintptr_t GetModuleAddress(const wchar_t* moduleName) const {
		MODULEENTRY32W entry{};
		entry.dwSize = sizeof(MODULEENTRY32W);

		const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, this->id);

		if (snapShot == INVALID_HANDLE_VALUE) {
			DWORD error = GetLastError();
			std::cout << "Module snapshot failed: " << error << std::endl;
			return 0;
		}

		uintptr_t result = 0;

		while (Module32NextW(snapShot, &entry)) {
			if (!wcscmp(moduleName, entry.szModule)) {
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
		T value{};
		ReadProcessMemory(this->process, (LPCVOID)address, &value, sizeof(T), NULL);
		return value;
	}

	template <typename T>
	bool Write(uintptr_t address, T value) const {
		return WriteProcessMemory(this->process, (LPVOID)address, &value, sizeof(T), NULL);
	}

	void Inspect(uintptr_t address, size_t nBytes) const {
		size_t nBytesRead = 0;
		byte* data = new byte[nBytes];

		bool ret = ReadProcessMemory(this->process, (LPCVOID)address, data, nBytes, &nBytesRead);

		if (!ret) {
			std::cout << "Inspect " << nBytes << " bytes at 0x" << std::hex << address << std::dec << " failed." << std::endl;

			delete[] data;
			return;
		}

		std::ostringstream byteString{};
		
		byteString << std::hex;

		for (size_t i = 0; i < nBytesRead; i += BYTES_PER_ROW)
		{
			for (size_t j = 0; (i + j < nBytesRead) and j < BYTES_PER_ROW; j++)
			{
				if (i + j >= nBytes) {
					std::cout << "Read over the limit." << std::endl;
					exit(-1);
				}

				byteString << std::setw(2) << std::setfill('0') << (unsigned int)data[i + j] << " ";
			}
			byteString << std::endl;
		}
		byteString << std::dec;
		
		if (nBytesRead != nBytes) {
			byteString << "Inspect read " << nBytesRead << "/" << nBytes << " bytes." << std::endl;
		}

		std::cout << byteString.str();

		delete[] data;
	}
};
