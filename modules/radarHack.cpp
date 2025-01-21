#pragma once

#include "cheats.hpp"

#include <thread>

#include "../shared/defines.hpp"
#include "../shared/sharedVariables.hpp"
#include "../shared/offsets.hpp"

#include "../entityListInteraction.hpp"


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
	while (true)
	{
		if (!activeCheats.radar.load())
		{
			std::this_thread::sleep_for(delays::cheatInactiveWaitTime);
			continue;
		}

		if (!*mem) {
			std::this_thread::sleep_for(delays::memConnectionWaitTime);
			continue;
		}

		std::this_thread::sleep_for(delays::radarHack::sleep);

		const auto client = (*mem)->GetModuleAddress(modules::client);

		const auto localPlayer = (*mem)->Read<uintptr_t>(client + offsets::localPlayerPawn);

		// No local player -> No match currently going on
		if (!localPlayer)
			continue;

		runRadarHack(**mem, client);
	}
}
