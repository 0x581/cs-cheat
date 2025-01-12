#pragma once

#include <thread>

#include "activeCheats.hpp"

#include "memory.hpp"
#include "../offset/offsets.hpp"

void runAntiFlashLoop(Memory **mem)
{
    while (activeCheats.antiFlash.load())
    {
        if (!*mem) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        const auto client = (*mem)->GetModuleAddress(L"client.dll");

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        const auto localPlayer = (*mem)->Read<uintptr_t>(client + offsets::localPlayerPawn);

        // No local player -> No match currently going on
        if (!localPlayer)
            continue;

        (*mem)->Write<float>(localPlayer + offsets::flashDuration, 0);
    }
}
