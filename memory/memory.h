#pragma once

#include <thread>

#include <Windows.h>
#include <iostream>
#include <sstream>
#include <iomanip>

#include <unordered_map>

#include <mutex>
#include <cassert>

#include "driver.h"

#include "../shared/defines.hpp"
#include "../hook/hijack_handle.hpp"

struct WCharHash {
	size_t operator()(const wchar_t* const str) const noexcept {
		size_t hash = 0;

		auto len = std::wcslen(str);

		for (size_t i = 0; i < len; ++i)
		{
			hash += hash * 65599 + str[i];
		}

		return hash ^ (hash >> 16);
	}
};

struct WCharEqual {
	bool operator()(const wchar_t* const lhs, const wchar_t* const rhs) const noexcept {
		return std::wcscmp(lhs, rhs) == 0;
	}
};

constexpr auto BYTES_PER_ROW = 32;


class Memory {
private:
	std::mutex mutex{};


	HANDLE driver_handle = NULL;


	std::unordered_map<const wchar_t*, uintptr_t, WCharHash, WCharEqual> modules{};

	bool loadModuleMap() {

		if (!driver::process_is_alive(driver_handle))
		{
			std::cout << "Tried to read modules of invalid process." << std::endl;
			exit(exit_codes::MEMORY_READ_MODULES_ERROR);
		}


		const auto requiredModules = std::vector<const wchar_t*>({ modules::client });

		size_t tries = 0;

		for (auto& module : requiredModules)
		{
			uintptr_t address = 0;

			do
			{
				address = driver::get_module_base(driver_handle, module);

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

			this->modules[module] = address;
		}

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
	bool is_valid{ false };

	const bool processIsAlive() {
		this->is_valid = this->is_valid && driver::process_is_alive(driver_handle);

		return this->is_valid;
	}

	Memory() {
		const HANDLE driver = CreateFile(L"\\\\.\\AssPileDriver", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (driver == INVALID_HANDLE_VALUE)
		{
			std::cout << "Failed to create our driver handle.\n";
			std::cin.get();

			throw std::runtime_error("Failed to create driver handle");
		}

		const auto old_debug_level = driver::set_debug_flags(driver, DEFAULT_FLAGS);
	}

	~Memory() {
		driver::unattach_from_process(driver_handle);

		CloseHandle(driver_handle);

		clearModuleMap();
	}

	const bool connectToProcess(const wchar_t* processName, const std::vector<const wchar_t*>& requiredModules) {
		const std::scoped_lock sl{ this->mutex };

		// Clean up previous connection if exists
		assert(!this->is_valid && "Cannot connect to process while already connected.");

		clearModuleMap();

		if (driver::attach_to_process(driver_handle, processName))
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

	const uintptr_t GetModuleAddress(const wchar_t* moduleName) const {
		if (!modules.contains(moduleName))
			std::wcout << "Failed to find module: " << moduleName << std::endl;

		return modules.at(moduleName);
	}

	template <typename T>
	T Read(uintptr_t address) const {
		return driver::read_memory<T>(driver_handle, address);
	}

	// Returns number of bytes read
	size_t ReadBytes(uintptr_t address, byte* buffer, size_t nBytes) const {
		return driver::read_memory(driver_handle, address, buffer, nBytes);
	}

	template <typename T>
	bool Write(uintptr_t address, T value) const {
		return driver::write_memory<T>(driver_handle, address, value);
	}

	void Inspect(uintptr_t address, size_t nBytes) const {
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
};
