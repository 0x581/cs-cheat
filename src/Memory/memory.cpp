#include "memory.hpp"

#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

#include "Windows.h"

#include "asspiledriver.h"

constexpr auto BYTES_PER_ROW = 32;

namespace delays
{
    namespace memory
    {
        constexpr auto moduleFetchWaitTime = std::chrono::seconds(1);
    }
}

Memory::Memory(std::vector<ModuleEntry> required_modules) : pid{0}, required_modules{required_modules}
{
    driver_handle = apd::open_driver();

    if (driver_handle == INVALID_HANDLE_VALUE)
    {
        MapDriver::map_driver();
        driver_handle = apd::open_driver();
    }

    if (driver_handle == INVALID_HANDLE_VALUE)
    {
        const auto error = GetLastError();

        std::cout << "Failed to create our driver handle: " << error << std::endl;

        throw std::runtime_error("Failed to create driver handle");
    }

    const auto old_debug_level = apd::set_debug_flags(driver_handle, 0);
}

Memory::~Memory()
{
    apd::detach_from_process(driver_handle);

    apd::close_driver(driver_handle);

    clearModuleMap();
}

bool Memory::loadModuleMap()
{
    size_t tries = 0;

    for (auto &[module_id, module_name] : required_modules)
    {
        uintptr_t address = 0;

        do
        {
            address = apd::get_module_base(driver_handle, module_name);

            if (address == 0)
            {
                std::this_thread::sleep_for(delays::memory::moduleFetchWaitTime);
                ++tries;
            }

        } while (address == 0 && tries < 10);

        if (tries >= 10)
        {
            // 10 attempts to get the modules failed, clear process
            std::cout << "Failed to fetch required Modules." << std::endl;

            clearModuleMap();

            return false;
        }

        this->modules[module_id] = address;
    }

    return true;
}

void Memory::clearModuleMap()
{
    this->modules.clear();
}

const bool Memory::isProcessAlive()
{
    this->is_valid = this->is_valid && apd::is_process_alive(driver_handle);

    return this->is_valid;
}

const bool Memory::connectToProcess(const wchar_t *processName)
{

    // Clean up previous connection if exists
    assert(!this->is_valid && "Cannot connect to process while already connected.");

    clearModuleMap();

    if (apd::attach_to_process(driver_handle, processName, &pid))
        std::cout << "Attachment successful.\n";
    else
        return false;

    // Initialize the module hashmap
    if (!this->loadModuleMap())
    {
        std::cout << "Failed to load module map." << std::endl;
        return false;
    }

    this->is_valid = true;

    return true;
}

// Normal functions for access & Co.

const uintptr_t Memory::GetModuleAddress(const int moduleID) const
{
    return modules.at(moduleID);
}

void Memory::Inspect(uintptr_t address, size_t nBytes) const
{
    uint8_t *buffer = new uint8_t[nBytes];

    size_t nBytesRead = this->ReadBytes(address, buffer, nBytes);

    if (!nBytesRead)
    {
        std::cout << "Inspect " << nBytes << " bytes at 0x" << std::hex << address << std::dec << " failed." << std::endl;

        delete[] buffer;
        return;
    }

    std::ostringstream byteString{};

    byteString << std::hex;

    for (size_t i = 0; i < nBytesRead && i < nBytes; i++)
    {
        if (i % 8 == 0 && i % BYTES_PER_ROW != 0)
            byteString << " ";

        if (i % BYTES_PER_ROW == 0 && i != 0)
            byteString << std::endl
                       << std::hex;

        byteString << std::setw(2) << std::setfill('0') << buffer[i];
    }

    byteString << std::dec;

    if (nBytesRead != nBytes)
    {
        byteString << "Inspect read " << nBytesRead << "/" << nBytes << " bytes." << std::endl;
    }

    std::cout << byteString.str();

    delete[] buffer;
}
