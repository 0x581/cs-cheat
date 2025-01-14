#pragma once

#include <thread>

#include "sharedVariables.hpp"

#include "memory.hpp"
#include "../offsets/offsets.hpp"
#include "../defines.hpp"

void runAntiFlashLoop(Memory **mem)
{
    while (activeCheats.antiFlash.load())
    {
        if (!*mem) {
            std::this_thread::sleep_for(delays::memConnectionWaitTime);
            continue;
        }

        std::this_thread::sleep_for(delays::antiFlash::sleep);

        const auto client = (*mem)->GetModuleAddress(modules::client);

        const auto localPlayer = (*mem)->Read<uintptr_t>(client + offsets::localPlayerPawn);

        // No local player -> No match currently going on
        if (!localPlayer)
            continue;

        (*mem)->Write<float>(localPlayer + offsets::flashDuration, 0);
    }
}
