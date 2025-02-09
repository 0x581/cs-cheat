#include "cheats.hpp"

#include <thread>

#include "../shared/defines.hpp"
#include "../shared/sharedVariables.hpp"
#include "../shared/offsets.hpp"
#include "../shared/colors.hpp"
#include "../shared/string_utils.hpp"
#include "../config/config.hpp"

#include "../entityListInteraction.hpp"

#include "../math/math.hpp"
#include "../lib/pch.h"


static void runEsp(const Memory& mem, const uintptr_t client)
{
    const auto localPlayerPawn = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);
    const auto localPlayerTeam = mem.Read<PlayerTeam_t>(localPlayerPawn + offsets::teamNum);

    const auto localPlayerPos = mem.Read<Vector3>(localPlayerPawn + offsets::OldOrigin);

    const auto view_matrix = mem.Read<view_matrix_t>(client + offsets::ViewMatrix);

    espData.time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 64; i++) {
        espData.data[i].isValid = false;

        const auto controller = getPlayerControllerPtr(mem, i);
        const auto playerPawn = getPawnPtrFromControllerPtr(mem, controller);

        if (!playerPawn || playerPawn == localPlayerPawn)
            continue;

        const auto playerTeam = mem.Read<PlayerTeam_t>(playerPawn + offsets::teamNum);

        espData.data[i].isFriendly = localPlayerTeam == playerTeam;

        const auto health = mem.Read<int>(playerPawn + offsets::health);

        espData.data[i].health = health;

        if (health <= 0 || health > 100)
            continue;

        const auto movementServices = mem.Read<intptr_t>(playerPawn + offsets::movementServices);

        if (!movementServices)
            continue;


        char buffer[PLAYER_NAME_LENGTH]{ 0 };
        if (!mem.ReadBytes(controller + offsets::playerName, (byte*)buffer, PLAYER_NAME_LENGTH))
            continue;

        parseString(buffer, espData.data[i].playerName, PLAYER_NAME_LENGTH);

        const auto skeletonInstance = mem.Read<uintptr_t>(playerPawn + offsets::gameSceneNode);
        const auto boneArrayPtr = mem.Read<uintptr_t>(skeletonInstance + offsets::modelState + 0x80);
        const auto boneArray = mem.Read<CBoneArray>(boneArrayPtr);
        espData.data[i].bones = BoneData::fromVector(boneArray, view_matrix);

        // Esp box
        const auto crouchAmount = mem.Read<float>(movementServices + offsets::crouchAmount);

        Vector3 footPos = mem.Read<Vector3>(playerPawn + offsets::OldOrigin);
        Vector3 headPos = { footPos.x, footPos.y, footPos.z + 72.f};

        auto screenFootPos = worldToScreen(footPos, view_matrix);
        auto screenHeadPos = worldToScreen(headPos, view_matrix);

        const auto distance = (localPlayerPos - footPos).length();

        float width = (screenFootPos.y - screenHeadPos.y) / 2.4F;

        if (crouchAmount != 0.f)
        {
            headPos = { footPos.x, footPos.y, footPos.z + 72.f - (crouchAmount * 17.f) };
            screenHeadPos = worldToScreen(headPos, view_matrix);
        }

        espData.data[i].min = { screenHeadPos.x - width / 2,  screenHeadPos.y };
        espData.data[i].max = { screenHeadPos.x + width / 2, screenFootPos.y };
        espData.data[i].distance = distance;

        espData.data[i].isValid = true;
    }
}

void runEspLoop(Memory& mem)
{
    while (true)
    {
        if (!config.activeCheats.esp)
        {
            std::this_thread::sleep_for(delays::cheatInactiveWaitTime);
            continue;
        }

        if (!mem.isValid()) {
            std::this_thread::sleep_for(delays::memConnectionWaitTime);
            continue;
        }


        std::this_thread::sleep_for(delays::esp::sleep);

        const auto client = mem.GetModuleAddress(modules::client);

        const auto localPlayer = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);

        // No local player -> No match currently going on
        if (!localPlayer)
            continue;

        runEsp(mem, client);
    }
}
