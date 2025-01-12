#pragma once

#include <thread>

#include "activeCheats.hpp"

#include "../memory.hpp"
#include "../offsets/offsets.hpp"



void runRadarHack(const Memory& mem, const uintptr_t client)
{
	const auto localPlayer = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);

	const auto entityListPtr = mem.Read<uintptr_t>(client + offsets::entityList);
	//first entry 
	const auto listEntry = mem.Read<uintptr_t>(entityListPtr + 0x10);

	if (!listEntry)
		return;

	for (size_t i = 0; i < 64; i++)
	{
		// Get current controller
		auto currentController = mem.Read<uintptr_t>(listEntry + i * 0x78);

		if (currentController == 0)
			continue;

		auto pawnHandle = mem.Read<uintptr_t>(currentController + offsets::playerPawn);

		if (pawnHandle == 0)
			continue;

		auto listEntry2 = mem.Read<uintptr_t>(entityListPtr + 0x10 + 0x8 * ((pawnHandle & 0x7FFF) >> 9));
		auto currentPawn = mem.Read<uintptr_t>(listEntry2 + 0x78 * (pawnHandle & 0x1FF));

		mem.Write<bool>(currentPawn + offsets::entitySpottedState + offsets::spotted, true);

		//auto health = mem.Read<int>(currentPawn + offsets::health); for later if  needed
	}
}

void runRadarLoop(Memory **mem)
{
	while (activeCheats.radar.load())
	{
		if (!*mem) {
			std::this_thread::sleep_for(delays::memConnectionWaitTime);
			continue;
		}

		std::this_thread::sleep_for(delays::radarHack::sleep);

		const auto client = (*mem)->GetModuleAddress(modules::client);

		if (!client)
			continue;

		const auto localPlayer = (*mem)->Read<uintptr_t>(client + offsets::localPlayerPawn);

		// No local player -> No match currently going on
		if (!localPlayer)
			continue;

		runRadarHack(**mem, client);
	}
}
