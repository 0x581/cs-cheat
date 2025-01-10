#pragma once

#include <thread>

#include "activeCheats.hpp"

#include "../memory.hpp"
#include "../offsets.hpp"

#ifdef ENABLE_BHOP


void static bunnyHop(const Memory* mem, uintptr_t client)
{
	//const auto onGroundLastTick = mem.Read<bool>(localPlayer + offsets::onGroundLastTick);

	static bool jump = false;
	if (!jump)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		mem->Write<int>(client + offsets::jump, 65537);
		jump = true;
	}
	if (jump)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		mem->Write<int>(client + offsets::jump, 256);
		jump = false;
	}
}

#else

static void bunnyHop(const Memory* mem, uintptr_t client) {}

#endif