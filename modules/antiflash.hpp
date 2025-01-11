#pragma once

#include <thread>


#include "activeCheats.hpp"

#include "memory.hpp"
#include "offsets.hpp"

#ifdef ENABLE_ANTIFLASH

void static antiFlash(const Memory &mem)
{
	const auto client = mem.GetModuleAddress(L"client.dll");

	const auto localPlayer = mem.Read<std::uintptr_t>(client + offsets::localPlayerPawn);

	mem.Write<float>(localPlayer + offsets::flashDuration, 0);
}

#else

static void antiflash(const Memory& mem) {}

#endif