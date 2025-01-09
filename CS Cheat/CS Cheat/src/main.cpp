#include "memory.h"

#include <thread>
#include <iostream>

// https://github.com/frk1/hazedumper/blob/master/csgo.hpp
namespace offsets
{
	//constexpr auto localPlayerController = 0x1A64E90;
	constexpr auto localPlayerPawn = 0x1868CF8;
	constexpr auto onGroundLastTick = 0x2298;
	constexpr auto movementServices = 0x11E8; // Parent: localPlayerPawn
	constexpr auto buttons = 0x48; // Parent: movementServices
	constexpr auto jump = 0x1861A00; // Parent: buttons
}

struct BIG_data {
	size_t a;
	size_t b;
	size_t c;
	size_t d;
};

int main()
{
	const auto mem = Memory("cs2.exe");

	std::cout << "CS2 PID: " << mem.getProcessId() << std::endl;

	const auto client = mem.GetModuleAddress("client.dll");
	std::cout << "client.dll -> " << "0x" << std::hex << client << std::dec << std::endl;

	while (true)
	{
		// we don't need this running a billion times per second :)
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		const auto localPlayer = mem.Read<std::uintptr_t>(client + offsets::localPlayerPawn);

		if (!localPlayer)
			continue;

		const auto onGroundLastTick = mem.Read<bool>(localPlayer + offsets::onGroundLastTick);

		const auto movementService = mem.Read<std::uintptr_t>(localPlayer + offsets::movementServices);

		const auto buttons = mem.Read<BIG_data>(movementService + offsets::buttons);

		std::cout << buttons.a << buttons.b << buttons.c << buttons.d << std::endl;

		// bhop
		if (GetAsyncKeyState(VK_SPACE))
		{
			if (onGroundLastTick) {
				mem.Write<std::uintptr_t>(client + offsets::jump, 65537);
			} else {
				mem.Write<std::uintptr_t>(client + offsets::jump, 16777472);
			}
		}
	}
}