#pragma once

#include <thread>

#include "sharedVariables.hpp"

#include "../memory.hpp"
#include "../offsets/offsets.hpp"
#include "../defines.hpp"

static void runFovChangerLoop(Memory** mem)
{
    // const auto onGroundLastTick = mem.Read<bool>(localPlayer + offsets::onGroundLastTick);

    while (activeCheats.fovChanger.load())
    {
        if (!*mem) {
            std::this_thread::sleep_for(delays::memConnectionWaitTime);
            continue;
        }

        std::this_thread::sleep_for(delays::bunnyHop::sleep);

        const auto client = (*mem)->GetModuleAddress(modules::client);

        const auto localPlayer = (*mem)->Read<uintptr_t>(client + offsets::localPlayerPawn);

        // No local player -> No match currently going on
        if (!localPlayer)
            continue;

        int desiredFov = fovController.desiredFov.load();

        auto cameraservices = (*mem)->Read<uintptr_t>(localPlayer + offsets::CameraServices);

        auto currentFov = (*mem)->Read<UINT>(cameraservices + offsets::FOV);

        std::cout << "FOV: " << currentFov << std::endl;
        (*mem)->Write<UINT>(cameraservices + offsets::FOV, desiredFov);
        bool isScoped = (*mem)->Read<bool>(localPlayer + offsets::IsScoped);

        if (isScoped == true) {
            std::cout << "is scoped" << std::endl;
        }
        else {
            std::cout << "is not scoped" << std::endl;
        }

        //if (!isScoped && currentFov != desiredFov)
        //{
        //    (*mem)->Write<UINT>(cameraservices + offsets::FOV, desiredFov);
        //}
    }
}
