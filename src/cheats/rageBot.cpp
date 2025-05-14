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

static void releaseRageShot() {
    std::this_thread::sleep_for(std::chrono::milliseconds(config.aimBot.shotDelay)); // shotDelay in ms in deiner Config!
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

static void runRageAimbot(const Memory& mem, const uintptr_t client) {
    const auto localPlayerPawn = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);
    if (!localPlayerPawn) return;
    const auto localPlayerTeam = mem.Read<PlayerTeam_t>(localPlayerPawn + offsets::teamNum);

    Vector2 screenCenter{ (float)GetSystemMetrics(SM_CXSCREEN) / 2.f, (float)GetSystemMetrics(SM_CYSCREEN) / 2.f };

    auto lowestDistance = HUGE_VAL;
    ImVec2 closestTargetScreen{ 0, 0 };
    int closestPlayerPawn = 0;

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

        ImVec2 targetScreen = { 0, 0 };
        for (int j = 0; j < 64; ++j) {
            if (espData.data[j].isValid && espData.data[j].pawnPtr == playerPawn) {
                switch (config.aimBot.hitbox) {
                case 0: targetScreen = espData.data[j].bones.head; break;
                case 1: targetScreen = espData.data[j].bones.neck; break;
                case 2: targetScreen = espData.data[j].bones.spine1; break;
                default: targetScreen = espData.data[j].bones.head; break;
                }
                break;
            }
        }
        if (targetScreen.x == 0.f && targetScreen.y == 0.f)
            continue;

        // Sichtbarkeitspr¸fung (wie im legit)
        uint32_t spottedByMask0 = mem.Read<uint32_t>(
            playerPawn + offsets::entitySpottedState + offsets::SpottedByMask
        );
        uint32_t spottedByMask1 = mem.Read<uint32_t>(
            playerPawn + offsets::entitySpottedState + offsets::SpottedByMask + 4
        );
        const auto localPlayerController = mem.Read<uintptr_t>(client + offsets::localPlayerController);
        const int localPawnIndex = mem.Read<int>(localPlayerController + offsets::IDEntIndex);

        bool isVisible = false;
        if (localPawnIndex < 32)
            isVisible = (spottedByMask0 & (1U << localPawnIndex)) != 0;
        else
            isVisible = (spottedByMask1 & (1U << (localPawnIndex - 32))) != 0;

        // Rage: Optional, Sichtbarkeitspr¸fung kann ignoriert werden!
        if (config.rageBot.visible && !isVisible)
            continue;

        double distance = std::pow(targetScreen.x - screenCenter.x, 2) + std::pow(targetScreen.y - screenCenter.y, 2);

        if (distance >= lowestDistance)
            continue;

        lowestDistance = distance;
        closestTargetScreen = targetScreen;
        closestPlayerPawn = playerPawn;
    }

    if (lowestDistance == HUGE_VAL)
        return;

    const auto radius = config.rageBot.pixelRadius;
    const auto maxDistance = radius * radius;

    if (lowestDistance > maxDistance)
        return;

    const auto diff_x = closestTargetScreen.x - screenCenter.x;
    const auto diff_y = closestTargetScreen.y - screenCenter.y;

    const auto speedFactor = config.rageBot.speedFactor;

    if (speedFactor <= 0.001f) return;

    long new_x = static_cast<long>(diff_x * speedFactor);
    long new_y = static_cast<long>(diff_y * speedFactor);

    // Aggressiv: Sofort schieﬂen, wenn Fadenkreuz auf Ziel
    if (std::abs(diff_x) < 2 && std::abs(diff_y) < 2) {
        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
        std::thread(releaseRageShot).detach();
    }

    if (new_x != 0 || new_y != 0)
    {
        mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(new_x), static_cast<DWORD>(new_y), 0, 0);
    }
}

void runRageAimbotLoop(Memory& mem) {
    while (true)
    {
        if (!config.activeCheats.rageBot)
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

        if (!localPlayer)
            continue;

        if (GetAsyncKeyState(keybinds::rageBot))
            runRageAimbot(mem, client);
    }
}
