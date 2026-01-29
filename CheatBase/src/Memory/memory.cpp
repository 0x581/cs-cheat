#include "memory.hpp"

#include <cassert>
#include <format>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

#include "Windows.h"

#include "asspiledriver.h"

constexpr auto BYTES_PER_ROW = 32;

Memory::Memory() : driver_handle{INVALID_HANDLE_VALUE}, pid{0}, process_name{}, required_modules{}, modules{}, connected{false}
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

    apd::set_debug_flags(driver_handle, 0);
}

Memory::~Memory()
{
    apd::detach_from_process(driver_handle);

    apd::close_driver(driver_handle);

    clearModuleMap();
}

bool Memory::connectToProcess(const wchar_t *processName)
{

    // Clean up previous connection if exists
    assert(!this->connected && "Cannot connect to process while already connected.");

    this->process_name = processName;

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

    this->connected = true;

    return true;
}

bool Memory::reconnectIfNeeded()
{
    const auto previous = this->connected;
    auto alive = this->isProcessAlive();

    if (previous && !alive)
        std::wcout << std::format(L"Connection to {} lost.", this->process_name) << std::endl;

    if (!alive)
    {
        alive = this->connectToProcess(this->process_name.c_str());

        if (alive)
            std::wcout << std::format(L"Connection to {} established.", this->process_name) << std::endl;
    }

    return alive;
}

bool Memory::isProcessAlive()
{
    this->connected = this->connected && apd::is_process_alive(driver_handle);

    return this->connected;
}

// Normal functions for access & Co.

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
