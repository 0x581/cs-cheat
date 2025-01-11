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
	mem.Write<float>(localPlayer + offsets::FlashDuration, 0);
	uintptr_t listEntry = mem.Read<uintptr_t>(entityList + 0x10);
	for (int i = 0; i < 64; i++)
	{
		if (listEntry == 0)// skip if invalid
			continue;
		//get current controller
		uintptr_t currentController = mem.Read<uintptr_t>(listEntry + i * 0x78);
		if (currentController == 0)
			continue; // if invalid skip
		int pawnHandle = mem.Read<int>(currentController + offsets::m_hPlayerPawn);
		if (pawnHandle == 0)
			continue;
		uintptr_t listEntry2 = mem.Read<uintptr_t>(entityList + 0x8 * ((pawnHandle & 0x7FFF) >> 9) + 0x10);
		uintptr_t currentPawn = mem.Read<uintptr_t>(listEntry2 + 0x78 * (pawnHandle & 0x1FF));
		int team = mem.Read<int>(localPlayer + offsets::teamNum);
		int entIndex = mem.Read<int>(localPlayer + offsets::IDEntIndex);
		if (entIndex != -1)
		{
			int entityTeam = mem.Read<int>(currentPawn + offsets::teamNum);
			if (team != entityTeam)// if entity is enemy
			{
					mem.Write<int>(client + offsets::attack, 65537);//+ attack
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
					mem.Write<int>(client + offsets::attack, 256);//-attack
			}

		}
	}
}

#else
void glowHack(const Memory* mem, uintptr_t client) {}
#endif
