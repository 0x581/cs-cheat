#pragma once

#include "activeCheats.hpp"
#include <thread>
#include "../memory.hpp"
#include "../offsets.hpp"

#ifdef ENABLE_TRIGGERBOT

void static triggerbot(const Memory& mem)
{
	const auto client = mem.GetModuleAddress(L"client.dll");
	const auto entityList = mem.Read<std::uintptr_t>(client + offsets::entityList);
	const auto localPlayer = mem.Read<std::uintptr_t>(client + offsets::localPlayerPawn);
	mem.Write<float>(localPlayer + offsets::flashDuration, 0);
	uintptr_t listEntry = mem.Read<uintptr_t>(entityList + 0x10);

	if (listEntry == 0) // skip if invalid
		return;

	for (size_t i = 0; i < 64; i++)
	{
		//get current controller
		uintptr_t currentController = mem.Read<uintptr_t>(listEntry + i * 0x78);

		if (currentController == 0)
			continue; // if invalid skip

		auto pawnHandle = mem.Read<int>(currentController + offsets::playerPawn);

		if (pawnHandle == 0)
			continue;

		auto listEntry2 = mem.Read<uintptr_t>(entityList + 0x8 * ((pawnHandle & 0x7FFF) >> 9) + 0x10);
		auto currentPawn = mem.Read<uintptr_t>(listEntry2 + 0x78 * (pawnHandle & 0x1FF));

		auto team = mem.Read<int>(localPlayer + offsets::teamNum);
		auto entIndex = mem.Read<int>(localPlayer + offsets::IDEntIndex);

		if (entIndex != -1)
		{
			auto entityTeam = mem.Read<int>(currentPawn + offsets::teamNum);
			if (team != entityTeam) // if entity is enemy
			{
					mem.Write<int>(client + offsets::attack, 65537);			// + attack
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
					mem.Write<int>(client + offsets::attack, 256);				// - attack
			}
		}
	}
}

#else
void static triggerbot(const Memory& mem) {}
#endif
