#pragma once

#include <thread>

#include "activeCheats.hpp"

#include "../memory.hpp"
#include "../offset/offsets.hpp"
#include "../defines.hpp"

static void runBunnyHopLoop(Memory **mem)
{
    // const auto onGroundLastTick = mem.Read<bool>(localPlayer + offsets::onGroundLastTick);

    while (activeCheats.bunnyHop.load())
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

        if (!GetAsyncKeyState(keybinds::bunnyHop))
            continue;

        (*mem)->Write<int>(client + offsets::jump, 256);
        std::this_thread::sleep_for(delays::bunnyHop::keyPressTime);
        (*mem)->Write<int>(client + offsets::jump, 65537);
    }
}
