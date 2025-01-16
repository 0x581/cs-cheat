#pragma once

#include <thread>

#include "sharedVariables.hpp"

#include "../memory.hpp"
#include "../offsets/offsets.hpp"
#include "../defines.hpp"

static void runBunnyHopLoop(Memory **mem)
{
    // const auto onGroundLastTick = mem.Read<bool>(localPlayer + offsets::onGroundLastTick);

    while (true)
    {

        if (!activeCheats.bunnyHop.load())
        {
            std::this_thread::sleep_for(delays::cheatInactiveWaitTime);
            continue;
        }

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

        if (!GetAsyncKeyState(keybinds::bunnyHop))
            continue;

        (*mem)->Write<int>(client + offsets::jump, 256);
        std::this_thread::sleep_for(delays::bunnyHop::keyPressTime);
        (*mem)->Write<int>(client + offsets::jump, 65537);
    }
}
