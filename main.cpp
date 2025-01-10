#include "memory.hpp"
#include "offsets.hpp"

#include <thread>
#include <iostream>
#include <bitset>


#define bits(n) std::bitset<sizeof(n) * 8>(n)

namespace keybinds {
	constexpr auto slowMode = VK_MENU;
	constexpr auto bunnyHop = VK_SPACE;
}


void bunnyHop(const Memory *mem, uintptr_t client)
{
	//const auto onGroundLastTick = mem.Read<bool>(localPlayer + offsets::onGroundLastTick);

	static bool jump = false;
	if (!jump)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		mem->Write<int>(client + offsets::jump, 65537);
		jump = true;
	}
	if (jump)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		mem->Write<int>(client + offsets::jump, 256);
		jump = false;
	}
}

void radarHack(const Memory* mem, uintptr_t client)
{

}

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
		if (GetAsyncKeyState(keybinds::slowMode))
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		else
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

		const auto localPlayer = mem.Read<std::uintptr_t>(client + offsets::localPlayerPawn);

		// No local player -> No match currently going on
		if (!localPlayer)
			continue;

		const auto movementService = mem.Read<std::uintptr_t>(localPlayer + offsets::movementServices);

		mem.Inspect(movementService + offsets::buttons, 64);

		if (GetAsyncKeyState(keybinds::bunnyHop))
			bunnyHop(&mem, client);
	}
}