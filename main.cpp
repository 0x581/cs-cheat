#include "memory.hpp"
#include "offsets.hpp"

#include <thread>
#include <iostream>

//#include <bitset>
//#define bits(n) std::bitset<sizeof(n) * 8>(n)

#include "modules/bunnyHop.hpp"
#include "modules/antiflash.hpp"

namespace keybinds {
	constexpr auto slowMode = VK_MENU;
	constexpr auto bunnyHop = VK_SPACE;
}

constexpr auto DEFAULT_SLOW = false;


int main()
{
	const auto mem = Memory(L"cs2.exe");

	if (!mem.getProcessId() or !mem.getProcessHandle()) {
		std::cout << "Failed to find Counter Strike: 2\nPress any key to exit." << std::endl;
		(void) getchar();
		exit(-1);
	}

	const auto client = mem.GetModuleAddress(L"client.dll");

	std::cout << "Simple CS2 cheat running (PID: " << mem.getProcessId() << ")" << std::endl;
	
	while (true)
	{
		if (!GetAsyncKeyState(keybinds::slowMode) != !DEFAULT_SLOW)
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		else
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

		const auto localPlayer = mem.Read<std::uintptr_t>(client + offsets::localPlayerPawn);

		// No local player -> No match currently going on
		if (!localPlayer)
			continue;

		//const auto entityList = mem.Read<std::uintptr_t>(client + offsets::entityList);

		//std::cout << "LocalBasePlayerPawn address: 0x" << std::hex << localPlayer << std::dec << std::endl;
		//std::cout << "Entity List address: 0x" << std::hex << entityList << std::dec << std::endl;

		//mem.Inspect(entityList, 64);

		//std::cout << std::endl;

		if (GetAsyncKeyState(keybinds::bunnyHop))
			bunnyHop(&mem, client);
		//get entity list
		uintptr_t entityList = mem.Read<uintptr_t>(client + offsets::entityList);

		//first entry 
		uintptr_t listEntry = mem.Read<uintptr_t>(entityList +0x10);
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
			uintptr_t listEntry2 = mem.Read<uintptr_t>(entityList + 0x8 *((pawnHandle & 0x7FFF)>>9) + 0x10);

			uintptr_t currentPawn = mem.Read<uintptr_t>(listEntry2 + 0x78 *(pawnHandle & 0x1FF));

			//string name = mem.Read<string>(currentController + offsets::m_iszPlayerName);

			bool spotted = mem.Read<bool>(currentPawn + offsets::m_entitySpottedState + offsets::spotted);


			//string spottedStatus = spotted == true ? "spotted" : " ";
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}