#pragma once

#include <thread>

#include "activeCheats.hpp"

#include "../memory.hpp"
#include "../offset/offsets.hpp"
#include "../keybinds.hpp"

static void runBunnyHopLoop(Memory **mem)
{
    // const auto onGroundLastTick = mem.Read<bool>(localPlayer + offsets::onGroundLastTick);

    while (activeCheats.bunnyHop.load())
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

        if (!GetAsyncKeyState(keybinds::bunnyHop))
            continue;

        (*mem)->Write<int>(client + offsets::jump, 256);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        (*mem)->Write<int>(client + offsets::jump, 65537);
    }
}
