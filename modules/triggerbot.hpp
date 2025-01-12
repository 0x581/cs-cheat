#pragma once

#include <thread>

#include "activeCheats.hpp"

#include "../memory.hpp"
#include "../offset/offsets.hpp"
#include "../defines.hpp"


void runTriggerBot(const Memory &mem, const uintptr_t client)
{
    const auto entityList = mem.Read<uintptr_t>(client + offsets::entityList);
    const auto localPlayer = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);

    uintptr_t listEntry = mem.Read<uintptr_t>(entityList + 0x10);

    if (listEntry == 0) // skip if invalid
        return;

    for (size_t i = 0; i < 64; i++) {
        // Get current controller
        uintptr_t currentController = mem.Read<uintptr_t>(listEntry + i * 0x78);

        if (currentController == 0)
            continue; // if invalid skip

        auto pawnHandle = mem.Read<int>(currentController + offsets::playerPawn);

        if (pawnHandle == 0)
            continue;

        auto listEntry2 = mem.Read<uintptr_t>(entityList + 0x8 * ((pawnHandle & 0x7FFF) >> 9) + 0x10);
        auto currentPawn = mem.Read<uintptr_t>(listEntry2 + 0x78 * (pawnHandle & 0x1FF));

        // Get the local player's team and entity index
        auto team = mem.Read<int>(localPlayer + offsets::teamNum);
        auto entIndex = mem.Read<int>(localPlayer + offsets::IDEntIndex);

        if (entIndex == -1)
            continue;

        // Get the enemy's team (target entity)
        auto otherTeam = mem.Read<int>(currentPawn + offsets::teamNum);

        std::cout << "Own team: " << team << ", Other team: " << otherTeam << " (" << entIndex << ")" << std::endl;

        // Check if the target is an enemy (i.e. not on the same team)
        if (team != otherTeam) { // if entity is an enemy
            std::cout << "Gegner im Fadenkreuz! Triggerbot wird ausgelöst." << std::endl;

            // Angriff auslösen
            mem.Write<int>(client + offsets::attack, 65537); // + attack (Maus1 drücken)
            std::this_thread::sleep_for(delays::triggerBot::keyPressTime); // Warten
            mem.Write<int>(client + offsets::attack, 256); // - attack (Maus1 loslassen)
        }
    }

}

void runTriggerBotLoop(Memory **mem)
{
    while (activeCheats.triggerBot.load())
    {
        if (!*mem) {
            std::this_thread::sleep_for(delays::memConnectionWaitTime);
            continue;
        }

        std::this_thread::sleep_for(delays::triggerBot::sleep);

        const auto client = (*mem)->GetModuleAddress(L"client.dll");

        if (!client)
            continue;

        const auto localPlayer = (*mem)->Read<uintptr_t>(client + offsets::localPlayerPawn);

        // No local player -> No match currently going on
        if (!localPlayer)
            continue;

        if (GetAsyncKeyState(keybinds::triggerbot))
            runTriggerBot(**mem, client);
    }
}
