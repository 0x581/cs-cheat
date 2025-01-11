#include "memory.hpp"
#include "offsets.hpp"

#include <thread>
#include <iostream>

#include "modules/bunnyHop.hpp"
#include "modules/antiflash.hpp"
#include "modules/triggerbot.hpp"

int main()
{
	const auto mem = Memory(L"cs2.exe");

    if (!mem.getProcessId() || !mem.getProcessHandle())
    {
		std::cout << "Failed to find Counter Strike: 2\nPress any key to exit." << std::endl;
        (void)getchar();
		exit(-1);
	}

	const auto client = mem.GetModuleAddress(L"client.dll");

	std::cout << "Simple CS2 cheat running (PID: " << mem.getProcessId() << ")" << std::endl;
	
    std::thread triggerBot{runTriggerBotLoop, mem};
    std::thread antiFlash{runAntiFlashLoop, mem};
    std::thread bunnyHop{runBunnyHopLoop, mem};

	while (true)
	{
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

		const auto localPlayer = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);

		// No local player -> No match currently going on
		if (!localPlayer)
			continue;

        // get entity list
		auto entityListPtr = mem.Read<uintptr_t>(client + offsets::entityList);

        // first entry
		auto listEntry = mem.Read<uintptr_t>(entityListPtr + 0x10);

		if (!listEntry)
			continue;

		for (size_t i = 0; i < 64; i++)
		{
            // get current controller
			auto currentController = mem.Read<uintptr_t>(listEntry + i * 0x78);

			if (currentController == 0)
				continue; // if invalid skip

			auto pawnHandle = mem.Read<uintptr_t>(currentController + offsets::playerPawn);

			if (pawnHandle == 0)
				continue;

			auto listEntry2 = mem.Read<uintptr_t>(entityListPtr + 0x10 + 0x8 * ((pawnHandle & 0x7FFF) >> 9));

			auto currentPawn = mem.Read<uintptr_t>(listEntry2 + 0x78 * (pawnHandle & 0x1FF));

			// reads any health
			auto health = mem.Read<int>(currentPawn + offsets::health);

            // shows health
			std::cout << health << "hp ";
			
            // string name = mem.Read<string>(currentController + offsets::playerName);

            // bool spotted = mem.Read<bool>(currentPawn + offsets::m_entitySpottedState + offsets::spotted);
            // string spottedStatus = spotted == true ? "spotted" : " ";
		}

		std::cout << std::endl;
	}
}