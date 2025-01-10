#include "memory.hpp"

#include <thread>
#include <iostream>

namespace offsets {
	//constexpr auto localPlayerController = 0x1A64E90;
	constexpr auto localPlayerPawn = 0x1868CF8;
	constexpr auto onGroundLastTick = 0x2298;
	constexpr auto movementServices = 0x11E8; // Parent: localPlayerPawn
	constexpr auto buttons = 0x48; // Parent: movementServices
	constexpr auto jump = 0x1861A00; // In client
}

namespace keybinds {
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
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		const auto localPlayer = mem.Read<std::uintptr_t>(client + offsets::localPlayerPawn);

		// No local player -> No match currently going on
		if (!localPlayer)
			continue;

		//const auto movementService = mem.Read<std::uintptr_t>(localPlayer + offsets::movementServices);
		//const auto buttons = mem.Read<BIG_data>(movementService + offsets::buttons);
		//std::cout << buttons.a << buttons.b << buttons.c << buttons.d << std::endl;


		if (GetAsyncKeyState(keybinds::bunnyHop))
			bhop(&mem, client);
	}
}