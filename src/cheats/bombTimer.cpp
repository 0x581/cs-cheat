#include "cheats.h"

#include <thread>

#include "config.h"
#include "delays.h"
#include "entityListInteraction.h"
#include "offsets.h"
#include "shared_variables.h"


static void runBombHack(const Memory& mem, const uintptr_t client) {
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

	const auto localPlayer = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);

	const auto itemServices = mem.Read<uintptr_t>(localPlayer + offsets::itemServices);
	const auto hasDefuser = mem.Read<bool>(itemServices + offsets::hasDefuser);

	const auto defuseTime = hasDefuser ? 5.0 : 10.0;

	bombData.canBeDefused = timeRemaining > defuseTime;
	bombData.timeRemaining = timeRemaining;
	bombData.planted = true;
}

void runBombHackLoop(Memory& mem) {
	while (true)
	{
		if (!config.activeCheats.bombHack)
		{
			std::this_thread::sleep_for(delays::cheatInactiveWaitTime);
			continue;
		}

		if (!mem.is_valid)
		{
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
