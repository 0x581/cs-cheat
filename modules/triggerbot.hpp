#pragma once

#include <thread>

#include "activeCheats.hpp"

#include "../memory.hpp"
#include "../offsets/offsets.hpp"
#include "../defines.hpp"


void runTriggerBot(const Memory &mem, const uintptr_t client)
{
    const auto localPlayer = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);

    // Get the local player's team and entity index
    const auto team = mem.Read<uint8_t>(localPlayer + offsets::teamNum);
    const auto entIndex = mem.Read<CEntityIndex>(localPlayer + offsets::IDEntIndex);

    if (entIndex == -1)
        return;

    const auto entityListPtr = mem.Read<uintptr_t>(client + offsets::entityList);
    //first entry 
    const auto listEntry = mem.Read<uintptr_t>(entityListPtr + 0x10);

    if (!listEntry) // skip if invalid
        return;


    std::cout << "V3 Entity index: " << entIndex << std::endl;


    // Get current controller
    auto currentController = mem.Read<uintptr_t>(listEntry + (entIndex & 0x3F) * 0x78);

    mem.Inspect(listEntry + (entIndex & 0x3F) * 0x78, 32);

    if (currentController == 0)
        return;

    auto pawnHandle = mem.Read<uintptr_t>(currentController + offsets::playerPawn);

    if (pawnHandle == 0)
        return;

    auto listEntry2 = mem.Read<uintptr_t>(entityListPtr + 0x8 * ((pawnHandle & 0x7FFF) >> 9) + 0x10);
    auto currentPawn = mem.Read<uintptr_t>(listEntry2 + 0x78 * (pawnHandle & 0x1FF));

    // Get the enemy's team (target entity)
    auto otherTeam = mem.Read<uint8_t>(currentPawn + offsets::teamNum);

    std::cout << "Own team: " << (int)team << ", Other team: " << (int)otherTeam << " (" << entIndex << ")" << std::endl;

    // Check if the target is an enemy (i.e. not on the same team)
    if (team != otherTeam) { // if entity is an enemy
        std::cout << "Gegner im Fadenkreuz! Triggerbot wird ausgelöst." << std::endl;

        // Angriff auslösen
        mem.Write<int>(client + offsets::attack, 65537); // + attack (Maus1 drücken)
        std::this_thread::sleep_for(delays::triggerBot::keyPressTime); // Warten
        mem.Write<int>(client + offsets::attack, 256); // - attack (Maus1 loslassen)
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
