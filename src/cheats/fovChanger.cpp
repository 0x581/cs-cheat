#include "cheats.h"

#include <thread>

#include "config.h"
#include "delays.h"
#include "entityListInteraction.h"
#include "offsets.h"


void runFovChangerLoop(Memory& mem) {
	while (true)
	{
		if (!config.activeCheats.fovChanger)
		{
			std::this_thread::sleep_for(delays::cheatInactiveWaitTime);
			continue;
		}

		if (!mem.is_valid)
		{
			std::this_thread::sleep_for(delays::memConnectionWaitTime);
			continue;
		}

		std::this_thread::sleep_for(delays::fovChanger::sleep);

		const auto client = mem.GetModuleAddress(modules::client);

		const auto localPlayer = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);

		// No local player -> No match currently going on
		if (!localPlayer)
			continue;

		auto isScoped = mem.Read<bool>(localPlayer + offsets::IsScoped);

		if (isScoped)
			continue;

		auto cameraservices = mem.Read<uintptr_t>(localPlayer + offsets::CameraServices);

		if (!isScoped)
			mem.Write<UINT>(cameraservices + offsets::FOV, config.fov.desiredFov);
	}
}
