#include "cheats.h"

#include <thread>

#include "config.h"
#include "delays.h"
#include "entityListInteraction.h"
#include "offsets.h"


static void runRadarHack(const Memory& mem, const uintptr_t client) {
	for (size_t i = 0; i < 64; i++)
	{
		const auto entityPawnPtr = getPlayerPawnPtr(mem, i);

		if (!entityPawnPtr)
			continue;

		mem.Write<bool>(entityPawnPtr + offsets::entitySpottedState + offsets::spotted, true);
	}
}

void runRadarLoop(Memory& mem) {
	while (true)
	{
		if (!config.activeCheats.radar)
		{
			std::this_thread::sleep_for(delays::cheatInactiveWaitTime);
			continue;
		}

		if (!mem.is_valid)
		{
			std::this_thread::sleep_for(delays::memConnectionWaitTime);
			continue;
		}

		std::this_thread::sleep_for(delays::radarHack::sleep);

		const auto client = mem.GetModuleAddress(modules::client);

		const auto localPlayer = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);

		// No local player -> No match currently going on
		if (!localPlayer)
			continue;

		runRadarHack(mem, client);
	}
}
