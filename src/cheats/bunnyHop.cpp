#include "cheats.h"

#include <thread>

#include "config.h"
#include "delays.h"
#include "keybinds.h"
#include "modules.h"
#include "offsets.h"


void runBunnyHopLoop(Memory& mem) {
	// const auto onGroundLastTick = mem.Read<bool>(localPlayer + offsets::onGroundLastTick);

	while (true)
	{

		if (!config.activeCheats.bunnyHop)
		{
			std::this_thread::sleep_for(delays::cheatInactiveWaitTime);
			continue;
		}

		if (!mem.is_valid)
		{
			std::this_thread::sleep_for(delays::memConnectionWaitTime);
			continue;
		}

		std::this_thread::sleep_for(delays::bunnyHop::sleep);

		const auto client = mem.GetModuleAddress(modules::client);

		const auto localPlayer = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);

		// No local player -> No match currently going on
		if (!localPlayer)
			continue;

		if (!GetAsyncKeyState(keybinds::bunnyHop))
			continue;

		mem.Write<int>(client + offsets::jump, 256);
		std::this_thread::sleep_for(delays::bunnyHop::keyPressTime);
		mem.Write<int>(client + offsets::jump, 65537);
	}
}
