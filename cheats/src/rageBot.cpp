#include "cheat_entries.hpp"

#include <thread>
#include <cmath>

#include "config.hpp"
#include "cs_defines.hpp"
#include "CheatBase/vector_math.hpp"
#include "render_data.hpp"

static void releaseRageShot()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(cfg.config.aimBot.shotDelay)); // shotDelay in ms in deiner Config!
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

void runRageAimbot(const CheatData &cheatData)
{
    const auto &config = cfg.config;

    const auto localPlayerTeam = cheatData.localPlayer.team;

    Vector2 screenCenter{(float)GetSystemMetrics(SM_CXSCREEN) / 2.f, (float)GetSystemMetrics(SM_CYSCREEN) / 2.f};

    auto lowestDistance = HUGE_VAL;
    ImVec2 closestTargetScreen{0, 0};
    // int closestPlayerPawn = 0;

    for (CEntityIndex i = 0; i < 64; i++)
    {
        const auto playerEntity = cheatData.playerEntities[i];

        if (!playerEntity.isValid)
            continue;

        const auto playerTeam = playerEntity.team;

        if (config.aimBot.ignoreTeam && (localPlayerTeam == playerTeam))
            continue;

        const auto health = playerEntity.health;
        if (health <= 0 || health > 100)
            continue;

        ImVec2 targetScreen = {0, 0};

        const auto &espBoneData = renderData.esp.data[i].bones;

        switch (config.aimBot.hitbox)
        {
        case 0:
            targetScreen = espBoneData.head;
            break;
        case 1:
            targetScreen = espBoneData.neck;
            break;
        case 2:
            targetScreen = espBoneData.spine1;
            break;
        default:
            targetScreen = espBoneData.head;
            break;
        }

        if (targetScreen.x == 0.f && targetScreen.y == 0.f)
            continue;

        // Sichtbarkeitsprüfung (wie im legit)
        const auto spottedByMask = playerEntity.spottedByMask;

        const auto localPawnIndex = cheatData.localPlayer.lookAtEntityIndex;

        bool isVisible = (spottedByMask & (1ULL << localPawnIndex)) != 0;

        // Rage: Optional, Sichtbarkeitsprüfung kann ignoriert werden!
        if (config.rageBot.checkVisible && !isVisible)
            continue;

        double distance = std::pow(targetScreen.x - screenCenter.x, 2) + std::pow(targetScreen.y - screenCenter.y, 2);

        if (distance >= lowestDistance)
            continue;

        lowestDistance = distance;
        closestTargetScreen = targetScreen;
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

    if (speedFactor <= 0.001f)
        return;

    long new_x = static_cast<long>(diff_x * speedFactor);
    long new_y = static_cast<long>(diff_y * speedFactor);

    // Aggressiv: Sofort schießen, wenn Fadenkreuz auf Ziel
    if (std::abs(diff_x) < 2 && std::abs(diff_y) < 2)
    {
        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
        std::thread(releaseRageShot).detach();
    }

    if (new_x != 0 || new_y != 0)
    {
        mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(new_x), static_cast<DWORD>(new_y), 0, 0);
    }
}
