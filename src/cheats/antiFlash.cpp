#include "cheats.h"

#include <thread>

#include "config.h"
#include "delays.h"
#include "entityListInteraction.h"
#include "offsets.h"


void runAntiFlashLoop(Memory& mem) {
	while (true)
	{
		if (!config.activeCheats.antiFlash)
		{
			std::this_thread::sleep_for(delays::cheatInactiveWaitTime);
			continue;
		}

		if (!mem.is_valid)
		{
			std::this_thread::sleep_for(delays::memConnectionWaitTime);
			continue;
		}

		std::this_thread::sleep_for(delays::antiFlash::sleep);

		const auto client = mem.GetModuleAddress(modules::client);

		const auto localPlayer = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);

		// No local player -> No match currently going on
		if (!localPlayer)
			continue;

		mem.Write<float>(localPlayer + offsets::flashDuration, 0);
	}
}
