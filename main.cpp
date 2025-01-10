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

struct BIG_data {
	size_t a;
	size_t b;
	size_t c;
	size_t d;
};


void bhop(const Memory *mem, uintptr_t client)
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
		const auto buttons = mem.Read<BIG_data>(movementService + offsets::buttons);

		std::cout << bits(buttons.a) << bits(buttons.b) << std::endl;


		if (GetAsyncKeyState(keybinds::bunnyHop))
			bhop(&mem, client);
	}
}