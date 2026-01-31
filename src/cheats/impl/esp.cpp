#include "cheat_entries.hpp"

#include "CheatBase/vector_math.hpp"
#include "render_data.hpp"

#include "imgui.h"

void runEsp(const CheatData &cheatData)
{
    const auto localPlayerPawn = cheatData.localPlayer.pawnPtr;
    const auto localPlayerTeam = cheatData.localPlayer.team;

    const auto localPlayerPos = cheatData.localPlayer.oldOrigin;

    const auto view_matrix = cheatData.viewMatrix;

    for (int i = 0; i < 64; i++)
    {
        auto &espDataEntry = renderData.esp.data[i];

        espDataEntry.isValid = false;

        const auto &playerEntity = cheatData.playerEntities[i];

        const auto playerPawn = playerEntity.pawnPtr;
        const auto controller = playerEntity.controllerPtr;

        if (!playerPawn || playerPawn == localPlayerPawn)
            continue;

        const auto playerTeam = playerEntity.team;

        espDataEntry.isFriendly = localPlayerTeam == playerTeam;

        const auto health = playerEntity.health;

        espDataEntry.maxHealth = playerEntity.maxHealth;
        espDataEntry.health = health;

        if (health <= 0 || health > 100)
            continue;

        memcpy(espDataEntry.playerName, playerEntity.name, PLAYER_NAME_LENGTH);

        espDataEntry.bones = BoneData::fromVector(playerEntity.bones, view_matrix);

        // Esp box
        const auto crouchAmount = playerEntity.crouchAmount;

        Vector3 footPos = playerEntity.oldOrigin;
        Vector3 headPos = {footPos.x, footPos.y, footPos.z + 72.f};

        auto screenFootPos = worldToScreen(footPos, view_matrix);
        auto screenHeadPos = worldToScreen(headPos, view_matrix);

        const auto distance = (localPlayerPos - footPos).length();

        float width = (screenFootPos.y - screenHeadPos.y) / 2.4F;

        if (crouchAmount != 0.f)
        {
            headPos = {footPos.x, footPos.y, footPos.z + 72.f - (crouchAmount * 17.f)};
            screenHeadPos = worldToScreen(headPos, view_matrix);
        }

        espDataEntry.min = {screenHeadPos.x - width / 2, screenHeadPos.y};
        espDataEntry.max = {screenHeadPos.x + width / 2, screenFootPos.y};
        espDataEntry.distance = distance;

        espDataEntry.isValid = true;
    }
}
