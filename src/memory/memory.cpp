#pragma once

#include "memory.h"

#include <thread>

#include <Windows.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <unordered_map>

#include <cassert>

#include "asspiledriver.h"

#include "modules.h"
#include "delays.h"

constexpr auto BYTES_PER_ROW = 32;


bool Memory::loadModuleMap() {
	const auto requiredModules = std::vector<const wchar_t*>({ modules::client });

	size_t tries = 0;

	for (auto& module : requiredModules)
	{
		uintptr_t address = 0;

		do
		{
			address = apd::get_module_base(driver_handle, module);

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

			return 0;
		}

		const auto module_name_copy = _wcsdup(module);

		this->modules[module_name_copy] = address;
	}

	return true;
}

void Memory::clearModuleMap() {
	for (auto& module : this->modules)
	{
		delete[] module.first;
	}

	this->modules.clear();
}

const bool Memory::isProcessAlive() {
	this->is_valid = this->is_valid && apd::is_process_alive(driver_handle);

	return this->is_valid;
}

Memory::Memory() : pid{ 0 } {

	driver_handle = apd::open_driver();

	if (driver_handle == INVALID_HANDLE_VALUE)
	{
		const auto error = GetLastError();

		std::cout << "Failed to create our driver handle: " << error << std::endl;
		std::cin.get();

		throw std::runtime_error("Failed to create driver handle");
	}

	const auto old_debug_level = apd::set_debug_flags(driver_handle, 0);
}

Memory::~Memory() {
	apd::detach_from_process(driver_handle);

	apd::close_driver(driver_handle);

	clearModuleMap();
}

const bool Memory::connectToProcess(const wchar_t* processName, const std::vector<const wchar_t*>& requiredModules) {

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

const uintptr_t Memory::GetModuleAddress(const wchar_t* moduleName) const {
	return modules.at(moduleName);
}

// Returns number of bytes read
size_t Memory::ReadBytes(uintptr_t address, byte* buffer, size_t nBytes) const {
	return apd::read_memory(driver_handle, address, buffer, nBytes);
}

void Memory::Inspect(uintptr_t address, size_t nBytes) const {
	byte* buffer = new byte[nBytes];

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

			if (j % 4 == 0 && j != 0)
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
