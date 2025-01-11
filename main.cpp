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

		const auto localPlayer = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);

		// No local player -> No match currently going on
		if (!localPlayer)
			continue;

		if (GetAsyncKeyState(keybinds::bunnyHop))
			bunnyHop(mem);

		antiFlash(mem);

		//get entity list
		auto entityListPtr = mem.Read<uintptr_t>(client + offsets::entityList);

		//first entry 
		auto listEntry = mem.Read<uintptr_t>(entityListPtr + 0x10);

		if (!listEntry)
			continue;

		for (size_t i = 0; i < 64; i++)
		{
			//get current controller
			auto currentController = mem.Read<uintptr_t>(listEntry + i * 0x78);

			if (currentController == 0)
				continue; // if invalid skip

			auto pawnHandle = mem.Read<uintptr_t>(currentController + offsets::playerPawn);

			if (pawnHandle == 0)
				continue;

			std::cout << mem.Read<int>(listEntry + 0x344) << "HP ";

			auto listEntry2 = mem.Read<uintptr_t>(entityListPtr + 0x10 + 0x8 *((pawnHandle & 0x7FFF) >> 9));

			auto currentPawn = mem.Read<uintptr_t>(listEntry2 + 0x78 *(pawnHandle & 0x1FF));

			//string name = mem.Read<string>(currentController + offsets::playerName);

		}

		std::cout << std::endl;
	}
}