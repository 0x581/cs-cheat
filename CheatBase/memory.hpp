#pragma once

#include <span>
#include <unordered_map>
#include <cstdint>

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

    std::vector<ModuleEntry> required_modules;

    std::unordered_map<int, uintptr_t> modules{};

    bool loadModuleMap();
    void clearModuleMap();

public:
    bool is_valid{false};

    Memory(std::vector<ModuleEntry> modules);
    template <std::size_t N>
    Memory(const std::array<ModuleEntry, N> &arr) : Memory(std::vector(arr.begin(), arr.end())) {}
    Memory(Memory &&) = delete;
    Memory(const Memory &) = delete;
    ~Memory();

    const bool connectToProcess(const wchar_t *processName);
    const uintptr_t GetModuleAddress(const int moduleID) const;

    const bool isProcessAlive();

    // Normal functions for access & Co.

    template <typename T>
    T Read(uintptr_t address) const
    {
        T value{};
        const bool success = apd::read_memory(driver_handle, address, &value, sizeof(T));

        return success ? value : T{};
    }

    // THIS FUNCTION IS FOR DEBUGGING PURPOSES ONLY!!! DOES NOT CHECK VALIDITY AND DOES NOT CLEAN UP THE HEAP
    uint8_t *ReadDbg(uintptr_t address, size_t nBytes) const
    {
        auto *buffer = new uint8_t[nBytes];
        const size_t bytes_read = apd::read_memory(driver_handle, address, buffer, nBytes);

        return buffer;
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
