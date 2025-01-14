#pragma once

#include <thread>

#include "sharedVariables.hpp"

#include "../memory.hpp"
#include "../offsets/offsets.hpp"
#include "../defines.hpp"

static void runBunnyHopLoop(Memory** mem)
{
    // const auto onGroundLastTick = mem.Read<bool>(localPlayer + offsets::onGroundLastTick);

    while (activeCheats.fovChanger.load())
    {
        if (!*mem) {
            std::this_thread::sleep_for(delays::memConnectionWaitTime);
            continue;
        }

        std::this_thread::sleep_for(delays::bunnyHop::sleep);

        const auto client = (*mem)->GetModuleAddress(L"client.dll");

        if (!client)
            continue;

        const auto localPlayer = (*mem)->Read<uintptr_t>(client + offsets::localPlayerPawn);

        // No local player -> No match currently going on
        if (!localPlayer)
            continue;

        int desiredFov = fovController.desiredFov.load();

        auto cameraservices = (*mem)->Read<uintptr_t>(localPlayer + offsets::CameraServices);

        auto currentFov = (*mem)->Read<UINT>(cameraservices + offsets::FOV);

        std::cout << currentFov;
        (*mem)->Write<UINT>(cameraservices + offsets::FOV, desiredFov);
        bool isScoped = (*mem)->Read<bool>(localPlayer + offsets::IsScoped);
        if (isScoped = true) {
            std::cout << "is scoped";
        }
        else {
            std::cout << "is not scoped";
        }

        if (!isScoped && currentFov != desiredFov)
        {
            mem->Write<UINT>(cameraservices + offsets::FOV, desiredFov);
        }
    }
}
