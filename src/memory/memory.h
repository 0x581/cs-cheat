#pragma once

#include <Windows.h>

#include <unordered_map>

#include "asspiledriver.h"
#include <modules.h>

class Memory
{
private:
    HANDLE driver_handle;
    HANDLE pid;

    std::unordered_map<ModuleID, uintptr_t> modules{};

    bool loadModuleMap();
    void clearModuleMap();

public:
    bool is_valid{false};

    Memory();
    ~Memory();

    const bool connectToProcess(const wchar_t *processName);
    const uintptr_t GetModuleAddress(const ModuleID moduleName) const;

    const bool isProcessAlive();

    // Normal functions for access & Co.

    template <typename T>
    T Read(uintptr_t address) const
    {
        T value{};
        const bool success = apd::read_memory(driver_handle, address, &value, sizeof(T));

        return success ? value : T{};
    }

    template <typename T>
    size_t Write(uintptr_t address, T value) const
    {
        return apd::write_memory(driver_handle, address, &value, sizeof(T));
    }

    // Returns number of bytes read
    size_t ReadBytes(uintptr_t address, byte *buffer, size_t nBytes) const;

    void Inspect(uintptr_t address, size_t nBytes) const;
};
