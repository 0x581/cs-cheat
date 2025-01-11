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

constexpr auto DEFAULT_SLOW = true;


int main()
{
	const auto mem = Memory(L"cs2.exe");

	if (!mem.getProcessId() or !mem.getProcessHandle()) {
		std::cout << "Failed to find Counter Strike: 2\nPress any key to exit." << std::endl;
		(void) getchar();
		exit(-1);
	}

	const auto client = mem.GetModuleAddress(L"client.dll");

	if (!client) {
		std::cout << "Failed to find required module: client.dll\nPress any key to exit." << std::endl;
		(void)getchar();
		exit(-1);
	}

	std::cout << "Simple CS2 cheat running (PID: " << mem.getProcessId() << ")" << std::endl;

	while (true)
	{
		if (GetAsyncKeyState(keybinds::slowMode) ^ DEFAULT_SLOW)
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		else
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

		const auto localPlayer = mem.Read<std::uintptr_t>(client + offsets::localPlayerPawn);

		// No local player -> No match currently going on
		if (!localPlayer)
			continue;

		const auto movementService = mem.Read<std::uintptr_t>(localPlayer + offsets::movementServices);

		//working anti flash pls implement
		if (offsets::m_flFlashDuration > 0)
		{
			mem.Write<float>(localPlayer + 0x13F8, 0);
		}
		const auto entityList = mem.Read<std::uintptr_t>(client + offsets::entityList);

		std::cout << "LocalBasePlayerPawn address: 0x" << std::hex << localPlayer << std::dec << std::endl;
		std::cout << "Entity List address: 0x" << std::hex << entityList << std::dec << std::endl;

		mem.Inspect(movementService + offsets::buttons, 32);

		mem.Inspect(entityList, 64);

		std::cout << std::endl;

		if (GetAsyncKeyState(keybinds::bunnyHop))
			bunnyHop(&mem, client);
	}
}