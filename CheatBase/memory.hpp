#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "asspiledriver.h"

typedef std::pair<int, const wchar_t *> ModuleEntry;

namespace MapDriver
{
    void map_driver();
}

class Memory
{
private:
    HANDLE driver_handle;
    HANDLE pid;

    std::wstring process_name;

    std::vector<ModuleEntry> required_modules;

    std::unordered_map<int, uintptr_t> modules{};

    bool loadModuleMap();
    void clearModuleMap();

public:
    bool connected{false};

    Memory();
    Memory(Memory &&) = delete;
    Memory(const Memory &) = delete;
    ~Memory();

    void setRequiredModulesList(std::vector<ModuleEntry> modules);
    template <size_t N>
    void setRequiredModulesList(const std::array<ModuleEntry, N> modules)
    {
        this->setRequiredModulesList(std::vector<ModuleEntry>(modules.begin(), modules.end()));
    }

    uintptr_t GetModuleAddress(const int moduleID) const;

    bool connectToProcess(const wchar_t *processName);
    bool isProcessAlive();
    bool reconnectIfNeeded();

    // Normal functions for access & Co.

    template <typename T>
    T Read(uintptr_t address) const
    {
        T value{};
        const bool success = apd::read_memory(driver_handle, address, &value, sizeof(T));

        return success ? value : T{};
    }

    // Returns number of bytes read
    size_t ReadBytes(uintptr_t address, uint8_t *buffer, size_t nBytes) const
    {
        return apd::read_memory(driver_handle, address, buffer, nBytes);
    }

    template <typename T>
    size_t Write(uintptr_t address, T value) const
    {
        return apd::write_memory(driver_handle, address, &value, sizeof(T));
    }

    void Inspect(uintptr_t address, size_t nBytes) const;
};
