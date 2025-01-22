#include "cheats.hpp"

#include <thread>

#include "../shared/defines.hpp"
#include "../shared/sharedVariables.hpp"
#include "../shared/offsets.hpp"


void runFovChangerLoop(Memory** mem)
{
    while (true)
    {
        if (!activeCheats.fovChanger.load())
        {
            std::this_thread::sleep_for(delays::cheatInactiveWaitTime);
            continue;
        }

        if (!*mem) {
            std::this_thread::sleep_for(delays::memConnectionWaitTime);
            continue;
        }

        std::this_thread::sleep_for(delays::fovChanger::sleep);

        const auto client = (*mem)->GetModuleAddress(modules::client);

        const auto localPlayer = (*mem)->Read<uintptr_t>(client + offsets::localPlayerPawn);

        // No local player -> No match currently going on
        if (!localPlayer)
            continue;

        auto isScoped = (*mem)->Read<bool>(localPlayer + offsets::IsScoped);

        if (isScoped)
            continue;

        auto cameraservices = (*mem)->Read<uintptr_t>(localPlayer + offsets::CameraServices);

        if (!isScoped)
            (*mem)->Write<UINT>(cameraservices + offsets::FOV, fovData.desiredFov);
    }
}
