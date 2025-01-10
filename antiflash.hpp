#pragma once

#include <thread>


#include "memory.hpp"
#include "offsets.hpp"

#ifdef ENABLE_ANTIFLASH

void static antiflash(const Memory* mem, uintptr_t client)
{
	const auto localPlayer = mem.Read<std::uintptr_t>(client + offsets::localPlayerPawn);
	//working antiflash pls implement
	if (offsets::m_flFlashDuration > 0)
	{
		mem.Write<float>(localPlayer + 0x13F8, 0);
	}
}

#else

static void antiflash(const Memory* mem, uintptr_t client) {}

#endif