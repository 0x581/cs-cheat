#pragma once


#include <Windows.h>

#include <thread>
#include <unordered_map>

#include "asspiledriver.h"

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


class Memory {
private:
	HANDLE driver_handle;


	std::unordered_map<const wchar_t*, uintptr_t, WCharHash, WCharEqual> modules{};

	bool loadModuleMap();
	void clearModuleMap();

public:
	bool is_valid{ false };

	Memory();
	~Memory();

	const bool connectToProcess(const wchar_t* processName, const std::vector<const wchar_t*>& requiredModules);
	const uintptr_t GetModuleAddress(const wchar_t* moduleName) const;

	const bool isProcessAlive();

	// Normal functions for access & Co.

	template <typename T>
	T Read(uintptr_t address) const {
		T value{};
		const bool success = apd::read_memory(driver_handle, address, &value, sizeof(T));

		return success ? value : T{};
	}

	template <typename T>
	bool Write(uintptr_t address, T value) const {
		return apd::write_memory(driver_handle, address, &value, sizeof(T));
	}

	// Returns number of bytes read
	size_t ReadBytes(uintptr_t address, byte* buffer, size_t nBytes) const;

	void Inspect(uintptr_t address, size_t nBytes) const;
};
