#include "memory.hpp"
#include "offsets.hpp"

#include <thread>
#include <iostream>

//#include <bitset>
//#define bits(n) std::bitset<sizeof(n) * 8>(n)

#include "modules/bunnyHop.hpp"
#include "modules/antiflash.hpp"
#include "modules/triggerbot.hpp"
#include "modules/radarHack.hpp"

namespace keybinds {
	constexpr auto slowMode = VK_MENU;
	constexpr auto bunnyHop = VK_SPACE;
	constexpr auto triggerbot = VK_RBUTTON;
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

	const auto localPlayer = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);
	
	while (true)
	{
		if (!GetAsyncKeyState(keybinds::slowMode) != !DEFAULT_SLOW)
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		else
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

		// No local player -> No match currently going on
		if (!localPlayer)
			continue;

		if (GetAsyncKeyState(keybinds::triggerbot))
			triggerbot(mem);

		if (GetAsyncKeyState(keybinds::bunnyHop))
			bunnyHop(mem);

		antiFlash(mem);

		radarHack(mem);
		
		std::cout << std::endl;
	}
}