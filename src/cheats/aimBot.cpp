#include "cheats.h"

#include <thread>
#include <corecrt_math.h>
#include <cmath>

#include "config.h"
#include "cs_defines.h"
#include "delays.h"
#include "entityListInteraction.h"
#include "keybinds.h"
#include "vector_math.h"
#include "offsets.h"
#include "shared_variables.h" 

static void runAimBot(const Memory& mem, const uintptr_t client) {
    const auto localPlayerPawn = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);
    if (!localPlayerPawn) return;
    const auto localPlayerTeam = mem.Read<PlayerTeam_t>(localPlayerPawn + offsets::teamNum);

    Vector2 screenCenter{ (float)GetSystemMetrics(SM_CXSCREEN) / 2.f, (float)GetSystemMetrics(SM_CYSCREEN) / 2.f };

    auto lowestDistance = HUGE_VAL;
    ImVec2 closestHeadScreen{ 0, 0 };

    for (int i = 0; i < 64; i++)
    {
        const auto playerPawn = getPlayerPawnPtr(mem, i);

        if (!playerPawn || playerPawn == localPlayerPawn)
            continue;

        const auto playerTeam = mem.Read<PlayerTeam_t>(playerPawn + offsets::teamNum);

        if (config.aimBot.ignoreteam && (localPlayerTeam == playerTeam))
            continue;

        const auto health = mem.Read<int>(playerPawn + offsets::health);
        if (health <= 0 || health > 100)
            continue;

        ImVec2 headScreen = { 0, 0 };
        for (int j = 0; j < 64; ++j) {
            if (espData.data[j].isValid && espData.data[j].pawnPtr == playerPawn) {
                headScreen = espData.data[j].bones.head; 
                break;
            }
        }

        if (headScreen.x == 0.f && headScreen.y == 0.f)
            continue;

        double distance = std::pow(headScreen.x - screenCenter.x, 2) + std::pow(headScreen.y - screenCenter.y, 2);

        if (distance >= lowestDistance)
            continue;

        lowestDistance = distance;
        closestHeadScreen = headScreen;
    }

    if (lowestDistance == HUGE_VAL)
        return;

    const auto radius = config.aimBot.pixelRadius;
    const auto maxDistance = radius * radius;

    if (lowestDistance > maxDistance)
        return;

    const auto diff_x = closestHeadScreen.x - screenCenter.x;
    const auto diff_y = closestHeadScreen.y - screenCenter.y;

    const auto speedFactor = config.aimBot.speedFactor;

    if (speedFactor <= 0.001f) return;

    long new_x = static_cast<long>(diff_x * speedFactor);
    long new_y = static_cast<long>(diff_y * speedFactor);

    if (new_x != 0 || new_y != 0)
    {
        mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(new_x), static_cast<DWORD>(new_y), 0, 0);
    }
}

void runAimBotLoop(Memory& mem) {
    while (true)
    {
        if (!config.activeCheats.aimBot)
        {
            std::this_thread::sleep_for(delays::cheatInactiveWaitTime);
            continue;
        }

        if (!mem.is_valid)
        {
            std::this_thread::sleep_for(delays::memConnectionWaitTime);
            continue;
        }

        std::this_thread::sleep_for(delays::aimbot::sleep);

        const auto client = mem.GetModuleAddress(modules::client);

        const auto localPlayer = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);

        // No local player -> No match currently going on
        if (!localPlayer)
            continue;

        if (GetAsyncKeyState(keybinds::aimBot))
            runAimBot(mem, client);
    }
}
