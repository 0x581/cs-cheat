#pragma once

#include <thread>

#include "../shared/defines.hpp"
#include "../shared/sharedVariables.hpp"
#include "../shared/offsets.hpp"

#include "../memory.hpp"

#include "../entityListInteraction.hpp"

void runTriggerBot(const Memory &mem, const uintptr_t client)
{
    // Check if the target is an enemy (i.e. not on the same team)
    if (getCurrentLookatEnemyPawn(mem)) {
        // Angriff auslösen
        std::this_thread::sleep_for(delays::triggerBot::keyPressDelay); // Warten

        mem.Write<int>(client + offsets::attack, 65537); // + attack (Maus1 drücken)
        std::this_thread::sleep_for(delays::triggerBot::keyPressTime); // Warten
        mem.Write<int>(client + offsets::attack, 256); // - attack (Maus1 loslassen)
    }
}

void runTriggerBotLoop(Memory **mem)
{
    while (true)
    {
        if (!activeCheats.triggerBot.load())
        {
            std::this_thread::sleep_for(delays::cheatInactiveWaitTime);
            continue;
        }

        if (!*mem) {
            std::this_thread::sleep_for(delays::memConnectionWaitTime);
            continue;
        }

        std::this_thread::sleep_for(delays::triggerBot::sleep);

        const auto client = (*mem)->GetModuleAddress(modules::client);

        const auto localPlayer = (*mem)->Read<uintptr_t>(client + offsets::localPlayerPawn);

        // No local player -> No match currently going on
        if (!localPlayer)
            continue;

        if (GetAsyncKeyState(keybinds::triggerbot))
            runTriggerBot(**mem, client);
    }
}
