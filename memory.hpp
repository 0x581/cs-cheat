#pragma once

#include <Windows.h>
#include <iostream>
#include <sstream>
#include <iomanip>

#include <TlHelp32.h>
#include <unordered_map>

struct WCharHash
{
    size_t operator()(const wchar_t *const str) const noexcept
    {
        size_t hash = 0;

        auto len = std::wcslen(str);

        for (size_t i = 0; i < len; ++i)
        {
            hash += hash * 65599 + str[i];
        }

        return hash ^ (hash >> 16);
    }
};

// Custom equality function for wchar_t*
struct WCharEqual
{
    bool operator()(const wchar_t *const lhs, const wchar_t *const rhs) const noexcept
    {
        return std::wcscmp(lhs, rhs) == 0;
    }
};

constexpr auto BYTES_PER_ROW = 32;

class Memory
{

private:
    DWORD id = 0;          // PID
    HANDLE process = NULL; // Process handle

    std::unordered_map<const wchar_t *, uintptr_t, WCharHash, WCharEqual> modules{};

public:
    Memory(const wchar_t *processName)
    {
        PROCESSENTRY32W processEntry{};
        processEntry.dwSize = sizeof(PROCESSENTRY32W);

        const auto processSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (processSnapShot == INVALID_HANDLE_VALUE)
        {
            DWORD error = GetLastError();
            std::cout << "Process snapshot failed: " << error << std::endl;
            return;
        }

        while (Process32NextW(processSnapShot, &processEntry))
        {
            if (!wcscmp(processName, processEntry.szExeFile))
            {
                this->id = processEntry.th32ProcessID;
                this->process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->id);
                break;
            }
        }

        if (processSnapShot)
            CloseHandle(processSnapShot);

        if (!this->id || !this->process)
            return;

        MODULEENTRY32W moduleEntry{};
        moduleEntry.dwSize = sizeof(MODULEENTRY32W);

        const auto moduleSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, this->id);

        if (moduleSnapShot == INVALID_HANDLE_VALUE)
        {
            DWORD error = GetLastError();
            std::cout << "Module snapshot failed: " << error << std::endl;
            return;
        }

        while (Module32NextW(moduleSnapShot, &moduleEntry))
        {
            auto len = wcslen(moduleEntry.szModule) + 1;
            wchar_t *mNameCopy = new wchar_t[len];

            if (!wcscpy_s(mNameCopy, len, moduleEntry.szModule))
                modules[mNameCopy] = reinterpret_cast<uintptr_t>(moduleEntry.modBaseAddr);
            else
                delete[] mNameCopy;
        }

        if (moduleSnapShot)
            CloseHandle(moduleSnapShot);
    }

    ~Memory()
    {
        if (this->process)
            CloseHandle(this->process);

        for (auto &module : this->modules)
        {
            delete[] module.first;
        }

        this->modules.clear();
    }

    DWORD getProcessId() const { return this->id; }
    HANDLE getProcessHandle() const { return this->process; }

    uintptr_t GetModuleAddress(const wchar_t *moduleName) const
    {
        if (!modules.contains(moduleName))
        {
            std::wcout << "Failed to find module: " << moduleName << std::endl;
        }

        return modules.at(moduleName);
    }

    template <typename T>
    T Read(uintptr_t address) const
    {
        T value{};
        ReadProcessMemory(this->process, (LPCVOID)address, &value, sizeof(T), NULL);
        return value;
    }

    template <typename T>
    bool Write(uintptr_t address, T value) const
    {
        return WriteProcessMemory(this->process, (LPVOID)address, &value, sizeof(T), NULL);
    }

    void Inspect(uintptr_t address, size_t nBytes) const
    {
        size_t nBytesRead = 0;
        byte *data = new byte[nBytes];

        bool ret = ReadProcessMemory(this->process, (LPCVOID)address, data, nBytes, &nBytesRead);

        if (!ret)
        {
            std::cout << "Inspect " << nBytes << " bytes at 0x" << std::hex << address << std::dec << " failed." << std::endl;

            delete[] data;
            return;
        }

        std::ostringstream byteString{};

        byteString << std::hex;

        for (size_t i = 0; i < nBytesRead; i += BYTES_PER_ROW)
        {
            for (size_t j = 0; (i + j < nBytesRead) && j < BYTES_PER_ROW; j++)
            {
                if (i + j >= nBytes)
                {
                    std::cout << "Read over the limit." << std::endl;
                    exit(-1);
                }

                byteString << std::setw(2) << std::setfill('0') << (unsigned int)data[i + j] << " ";
            }
            byteString << std::endl;
        }
        byteString << std::dec;

        if (nBytesRead != nBytes)
        {
            byteString << "Inspect read " << nBytesRead << "/" << nBytes << " bytes." << std::endl;
        }

        std::cout << byteString.str();

        delete[] data;
    }
};
