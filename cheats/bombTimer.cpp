#pragma once

#include "cheats.hpp"

#include <thread>

#include "../shared/defines.hpp"
#include "../config/config.hpp"
#include "../shared/offsets.hpp"

#include "../shared/sharedVariables.hpp"

#include "../entityListInteraction.hpp"


static void runBombHack(const Memory& mem, const uintptr_t client)
{
	const auto c4IsPlanted = mem.Read<bool>(client + offsets::plantedC4 - 0x8);

	if (!c4IsPlanted)
	{
		bombData.planted = false;
		return;
	}

	const auto plantedC4Address = mem.Read<uintptr_t>(client + offsets::plantedC4);
	const auto plantedC4 = mem.Read<uintptr_t>(plantedC4Address);


	const auto globalVarsPtr = mem.Read<uintptr_t>(client + offsets::globalVars);
	const auto globalVars = mem.Read<CGlobalVarsBase>(globalVarsPtr);


	const auto explosionTime = mem.Read<GameTime_t>(plantedC4 + offsets::c4ExplosionTime);

	const auto timeRemaining = explosionTime - globalVars.currentTime;

	std::cout << timeRemaining << std::endl;

	bombData.timeRemaining = timeRemaining;
	bombData.planted = true;
}

void runBombHackLoop(Memory& mem)
{
	while (true)
	{
		if (!config.activeCheats.bombHack)
		{
			std::this_thread::sleep_for(delays::cheatInactiveWaitTime);
			continue;
		}

		if (!mem.isValid()) {
			std::this_thread::sleep_for(delays::memConnectionWaitTime);
			continue;
		}

		std::this_thread::sleep_for(delays::bombHack::sleep);

		const auto client = mem.GetModuleAddress(modules::client);

		const auto localPlayer = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);

		// No local player -> No match currently going on
		if (!localPlayer)
			continue;

		runBombHack(mem, client);
	}
}
