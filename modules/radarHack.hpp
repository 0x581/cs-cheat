#pragma once

#include <thread>

#include "activeCheats.hpp"

#include "../memory.hpp"
#include "../offsets/offsets.hpp"

#include "../offsets/entityListInteraction.hpp"


void runRadarHack(const Memory& mem, const uintptr_t client)
{
	for (size_t i = 0; i < 64; i++)
	{
		const auto entityPawnPtr = getPlayerPawnPtr(mem, i);

		if (!entityPawnPtr)
			continue;

		mem.Write<bool>(entityPawnPtr + offsets::entitySpottedState + offsets::spotted, true);
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
