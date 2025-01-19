#pragma once

#include <thread>

#include "../shared/defines.hpp"
#include "../shared/sharedVariables.hpp"
#include "../shared/offsets.hpp"
#include "../shared/colors.hpp"

#include "../memory.hpp"

#include "../math/math.hpp"
#include "../lib/pch.h"


#include "../entityListInteraction.hpp"


void runEsp(const Memory& mem, const uintptr_t client)
{
    const auto localPlayerPawn = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);
    const auto localPlayerTeam = mem.Read<uint8_t>(localPlayerPawn + offsets::teamNum);

    const auto view_matrix = mem.Read<view_matrix_t>(client + offsets::ViewMatrix);

    for (int i = 0; i < 64; i++) {
        const auto playerPawn = getPlayerPawnPtr(mem, i);

        if (!playerPawn)
        {
            delete espData.boxes[i];
            espData.boxes[i] = nullptr;
            continue;
        }

        const auto playerTeam = mem.Read<uint8_t>(playerPawn + offsets::teamNum);

        if (localPlayerTeam == playerTeam)
        {
            delete espData.boxes[i];
            espData.boxes[i] = nullptr;
            continue;
        }

        const int health = mem.Read<int>(playerPawn + offsets::health);

        if (health <= 0 || health > 100)
        {
            delete espData.boxes[i];
            espData.boxes[i] = nullptr;
            continue;
        }

        Vector3 footPos = mem.Read<Vector3>(playerPawn + offsets::OldOrigin);
        Vector3 headPos = { footPos.x, footPos.y, footPos.z + 75.f };

        Vector3 screenFootPos = footPos.WorldToScreen(view_matrix);
        Vector3 screenHeadPos = headPos.WorldToScreen(view_matrix);

        float height = (screenFootPos.y - screenHeadPos.y) / 8;
        float width = height / 2.4F;

        ImVec2 min{ screenHeadPos.x - width / 2,  screenHeadPos.y };
        ImVec2 max{ min.x + width, min.y + height };

        auto pair = new std::pair<ImVec2, ImVec2>(min, max);

        espData.boxes[i] = pair;

        std::cout << "(" << footPos.x << "|" << footPos.y << "|" << footPos.z << "), (" << screenFootPos.x << "|" << screenFootPos.y << ")" << std::endl;
    }
}

void runEspLoop(Memory** mem)
{
    while (true)
    {
        if (!activeCheats.esp.load())
        {
            std::this_thread::sleep_for(delays::cheatInactiveWaitTime);
            continue;
        }

        if (!*mem) {
            std::this_thread::sleep_for(delays::memConnectionWaitTime);
            continue;
        }

        std::this_thread::sleep_for(delays::esp::sleep);

        const auto client = (*mem)->GetModuleAddress(modules::client);

        const auto localPlayer = (*mem)->Read<uintptr_t>(client + offsets::localPlayerPawn);

        // No local player -> No match currently going on
        if (!localPlayer)
            continue;

        runEsp(**mem, client);
    }
}
