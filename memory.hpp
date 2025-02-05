#pragma once

#include <thread>

#include <Windows.h>
#include <iostream>
#include <sstream>
#include <iomanip>

#include <TlHelp32.h>
#include <unordered_map>

#include "shared/defines.hpp"
#include <mutex>
#include <cassert>

#include "hook/hijack_handle.hpp"

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
    bool is_valid{ false };
    std::mutex mutex{};


    HANDLE hProcess = NULL; // Process handle


    std::unordered_map<const wchar_t *, uintptr_t, WCharHash, WCharEqual> modules{};

    bool updateModuleMap() {

        if (!this->hProcess) {
            std::cout << "Tried to read modules of invalid process." << std::endl;
            exit(exit_codes::MEMORY_READ_MODULES_ERROR);
        }

        MODULEENTRY32W moduleEntry{};
        moduleEntry.dwSize = sizeof(MODULEENTRY32W);

        const auto moduleSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, GetProcessId(this->hProcess));

        if (moduleSnapShot == INVALID_HANDLE_VALUE)
        {
            DWORD error = GetLastError();
            std::cout << "Module snapshot failed: " << error << std::endl;
            return false;
        }

        while (Module32NextW(moduleSnapShot, &moduleEntry))
        {
            if (this->modules.contains(moduleEntry.szModule))
                continue;

            auto len = wcslen(moduleEntry.szModule) + 1;
            wchar_t* mNameCopy = new wchar_t[len];

            if (!wcscpy_s(mNameCopy, len, moduleEntry.szModule))
                modules[mNameCopy] = reinterpret_cast<uintptr_t>(moduleEntry.modBaseAddr);
            else
                delete[] mNameCopy;
        }

        if (moduleSnapShot)
            CloseHandle(moduleSnapShot);

        return true;
    }

    void clearModuleMap() {
        for (auto& module : this->modules)
        {
            delete[] module.first;
        }

        this->modules.clear();
    }

public:
    const bool processIsAlive() const {
        DWORD exitCode;
        return GetExitCodeProcess(this->hProcess, &exitCode) && (exitCode == STILL_ACTIVE);
    }

    Memory() {}

    ~Memory()
    {
        if (this->hProcess)
            CloseHandle(this->hProcess);

        clearModuleMap();
    }

    const bool connectToProcess(const wchar_t* processName, const std::vector<const wchar_t*>& requiredModules)
    {
        const std::scoped_lock sl{ this->mutex };

        // Clean up previous connection if exists
        assert(!this->is_valid && "Cannot connect to process while already connected.");

        if (this->hProcess)
            CloseHandle(this->hProcess);

        this->hProcess = 0;
        clearModuleMap();

        HANDLE pHandle = hijackHandle(processName, L"steam.exe");

        if (!pHandle)
            return false;

        this->hProcess = pHandle;

        // Initialize the module hashmap
        this->updateModuleMap();
        size_t try_num = 1;

        for (size_t i = 0; i < requiredModules.size();)
        {
            auto module = requiredModules[i];

            if (this->modules.contains(module)) {
                ++i;
                continue;
            }

            ++try_num;

            if (try_num < 10) {
                std::this_thread::sleep_for(delays::memory::moduleFetchWaitTime);
                this->updateModuleMap();
            }
            else {
                // 10 attempts to get the modules failed, clear process
                std::cout << "Failed to fetch required Modules." << std::endl;
                CloseHandle(this->hProcess);

                clearModuleMap();
                this->hProcess = 0;

                return false;
            }
        }

        this->hProcess = pHandle;
        this->is_valid = true;

        return true;
    }

    void invalidate()
    {
        const std::scoped_lock sl{ this->mutex };

        assert(this->is_valid && "Cannot invalidate Memory Object twice!");

        this->is_valid = false;

        if (this->hProcess)
            CloseHandle(this->hProcess);

        this->hProcess = 0;
    }

    const bool isValid()
    {
        const std::scoped_lock sl{ this->mutex };

        return this->hProcess && this->is_valid;
    }


    // Normal functions for access & Co.

    const HANDLE getProcessHandle() const { return this->hProcess; }

    const uintptr_t GetModuleAddress(const wchar_t *moduleName) const
    {
        if (!modules.contains(moduleName))
            std::wcout << "Failed to find module: " << moduleName << std::endl;

        return modules.at(moduleName);
    }

    template <typename T>
    T Read(uintptr_t address) const
    {
        T value{};

        if (this->processIsAlive())
            ReadProcessMemory(this->hProcess, (LPCVOID)address, &value, sizeof(T), NULL);

        return value;
    }

    // Returns number of bytes read
    size_t ReadBytes(uintptr_t address, byte *buffer, size_t nBytes) const
    {
        size_t nBytesRead{ 0 };

        if (nBytes && this->processIsAlive())
            ReadProcessMemory(this->hProcess, (LPCVOID)address, buffer, nBytes, &nBytesRead);

        return nBytesRead;
    }

    template <typename T>
    bool Write(uintptr_t address, T value) const
    {
        if (this->processIsAlive())
            return WriteProcessMemory(this->hProcess, (LPVOID)address, &value, sizeof(T), NULL);

        return false;
    }

    void Inspect(uintptr_t address, size_t nBytes) const
    {
        byte *buffer = new byte[nBytes];

        size_t nBytesRead = this->ReadBytes(address, buffer, nBytes);

        if (!nBytesRead)
        {
            std::cout << "Inspect " << nBytes << " bytes at 0x" << std::hex << address << std::dec << " failed." << std::endl;

            delete[] buffer;
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
                    exit(exit_codes::MEMORY_INSPECT_ERROR);
                }

                if (j % 8 == 0 && j != 0)
                    byteString << " ";

                byteString << std::setw(2) << std::setfill('0') << (unsigned int)buffer[i + j] << " ";
            }
            byteString << std::endl;
        }
        byteString << std::dec;

        if (nBytesRead != nBytes)
        {
            byteString << "Inspect read " << nBytesRead << "/" << nBytes << " bytes." << std::endl;
        }

        std::cout << byteString.str();

        delete[] buffer;
    }
};
