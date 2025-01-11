#pragma once

#include <thread>

#include "activeCheats.hpp"

#include "../memory.hpp"
#include "../offsets.hpp"

void runRadarHack(const Memory& mem, const uintptr_t client)
{
	const auto localPlayer = mem.Read<std::uintptr_t>(client + offsets::localPlayerPawn);
	//get entity list
	auto entityListPtr = mem.Read<uintptr_t>(client + offsets::entityList);
	//first entry 
	auto listEntry = mem.Read<uintptr_t>(entityListPtr + 0x10);

	if (!listEntry)
		return;

	for (size_t i = 0; i < 64; i++)
	{
		//get current controller
		auto currentController = mem.Read<uintptr_t>(listEntry + i * 0x78);

		if (currentController == 0)
			continue; // if invalid skip

		auto pawnHandle = mem.Read<uintptr_t>(currentController + offsets::playerPawn);

		if (pawnHandle == 0)
			continue; //if invalid skip

		auto listEntry2 = mem.Read<uintptr_t>(entityListPtr + 0x10 + 0x8 * ((pawnHandle & 0x7FFF) >> 9));

		auto currentPawn = mem.Read<uintptr_t>(listEntry2 + 0x78 * (pawnHandle & 0x1FF));

		mem.Write<bool>(currentPawn + offsets::entitySpottedState + offsets::spotted, true);

		//auto health = mem.Read<int>(currentPawn + offsets::health); for later if  needed
	}
}

void runRadarLoop(const Memory& mem)
{
	const auto client = mem.GetModuleAddress(L"client.dll");

	while (activeCheats.radar.load())
	{
		std::this_thread::sleep_for(std::chrono::seconds(10));

		const auto localPlayer = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);

		// No local player -> No match currently going on
		if (!localPlayer)
			continue;

		runRadarHack(mem, client);
	}
}
