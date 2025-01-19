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

	bombData.planted = true;

	for (int timeLeft = 40; timeLeft >= 0; timeLeft--) //count 40sec from bombplanted
	{
		bombData.timeRemaining = float(timeLeft);
		auto bombplanted = mem.Read<bool>(gameRules + offsets::BombPlanted); //refresh

		if (!bombplanted)// end loop if bomb is not planted anymore
			break;

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	bombData.planted = false;
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
