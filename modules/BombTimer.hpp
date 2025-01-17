#pragma once

#include <thread>

#include "../shared/defines.hpp"
#include "../shared/sharedVariables.hpp"
#include "../shared/offsets.hpp"

#include "../memory.hpp"


void runBombHack(const Memory& mem, const uintptr_t client)
{
	const auto gameRules = mem.Read<intptr_t>(client + offsets::GameRules);

	if (!gameRules) 
		return;

	auto bombplanted = mem.Read<bool>(gameRules + offsets::BombPlanted);

	if (!bombplanted)
		return;

	for (int timeleft = 40; timeleft >= 0; timeleft--) //count 40sec from bombplanted
	{
		auto bombplanted = mem.Read<bool>(gameRules + offsets::BombPlanted); //refresh

		if (!bombplanted)// end loop if bomb is not planted anymore
			break;

		std::cout << "Bomb planted: " << timeleft << " seconds remaining" << std::endl;

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

void runBombHackLoop(Memory** mem)
{
	while (true)
	{
		if (!activeCheats.bombHack.load())
		{
			std::this_thread::sleep_for(delays::cheatInactiveWaitTime);
			continue;
		}

		if (!*mem) {
			std::this_thread::sleep_for(delays::memConnectionWaitTime);
			continue;
		}

		std::this_thread::sleep_for(delays::bombHack::sleep);

		const auto client = (*mem)->GetModuleAddress(modules::client);

		const auto localPlayer = (*mem)->Read<uintptr_t>(client + offsets::localPlayerPawn);

		// No local player -> No match currently going on
		if (!localPlayer)
			continue;

		runBombHack(**mem, client);
	}
}
