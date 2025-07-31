#include "cheat_data.h"

#include <modules.h>
#include <offsets.h>
#include <entity_system.h>
#include <strings.h>
#include <vector_math.h>

void read_c4_data(const Memory &mem, const EntityList &entityList, C4Data &c4Data)
{
    const auto client = mem.GetModuleAddress(modules::client);

    if (!client)
        return;

    c4Data.planted = mem.Read<bool>(client + offsets::plantedC4 - 0x8);

    if (!c4Data.planted)
        return;

    const auto plantedC4Address = mem.Read<uintptr_t>(client + offsets::plantedC4);
    const auto plantedC4Ptr = mem.Read<uintptr_t>(plantedC4Address);

    c4Data.site = mem.Read<uint32_t>(plantedC4Ptr + offsets::c4BombSite);
    c4Data.explosionTime = mem.Read<GameTime_t>(plantedC4Ptr + offsets::c4ExplosionTime);
    c4Data.beingDefused = mem.Read<bool>(plantedC4Ptr + offsets::c4BeingDefused);
    c4Data.defused = mem.Read<bool>(plantedC4Ptr + offsets::c4Defused);

    const auto defuserHandle = mem.Read<CHandle>(plantedC4Ptr + offsets::c4Defuser);

    const auto defuserPawn = entityList.getAddressFromHandle(mem, defuserHandle);
}

bool read_player_data(const Memory &mem, const EntityList &entityList, const uintptr_t playerPawnPtr, PlayerData &playerData)
{
    if (!playerPawnPtr)
        return false;

    playerData.pawnPtr = playerPawnPtr;

    const auto controllerPtr = entityList.getControllerPtrFromPawnPtr(mem, playerPawnPtr);
    if (!controllerPtr)
        return false;

    playerData.controllerPtr = controllerPtr;
    playerData.team = mem.Read<PlayerTeam_t>(playerPawnPtr + offsets::teamNum);
    playerData.health = mem.Read<CHealth>(playerPawnPtr + offsets::health);
    playerData.maxHealth = mem.Read<CHealth>(playerPawnPtr + offsets::maxHealth);
    playerData.lookAtEntityIndex = mem.Read<CEntityIndex>(playerPawnPtr + offsets::IDEntIndex);

    memset(playerData.name, 0, PLAYER_NAME_LENGTH);
    mem.ReadBytes(controllerPtr + offsets::playerName, (byte *)playerData.name, PLAYER_NAME_LENGTH);

    cleanString(playerData.name, playerData.name, PLAYER_NAME_LENGTH);

    playerData.oldOrigin = mem.Read<Vector3>(playerPawnPtr + offsets::OldOrigin);

    const auto skeletonInstance = mem.Read<uintptr_t>(playerPawnPtr + offsets::gameSceneNode);
    const auto boneArrayPtr = mem.Read<uintptr_t>(skeletonInstance + offsets::modelState + 0x80);
    playerData.bones = mem.Read<CBoneArray>(boneArrayPtr);

    const auto movementServices = mem.Read<intptr_t>(playerPawnPtr + offsets::movementServices);
    playerData.crouchAmount = mem.Read<float>(movementServices + offsets::crouchAmount);

    playerData.spottedByMask = mem.Read<EntitySpottedState_t>(playerPawnPtr + offsets::entitySpottedState + offsets::spotted);

    const auto itemServices = mem.Read<uintptr_t>(playerPawnPtr + offsets::itemServices);
    playerData.hasDefuser = mem.Read<bool>(itemServices + offsets::hasDefuser);

    playerData.isScoped = mem.Read<bool>(playerPawnPtr + offsets::IsScoped);

    playerData.cameraServicesPtr = mem.Read<uintptr_t>(playerPawnPtr + offsets::CameraServices);

    return true;
}

bool read_cheat_data(const Memory &mem, const EntityList &entityList, CheatData &cheatData)
{
    memset(&cheatData, 0, sizeof(CheatData));

    const auto client = mem.GetModuleAddress(modules::client);

    if (!client)
        return false;

    const auto local_player_pawn = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);

    if (!local_player_pawn)
        return false;

    if (!read_player_data(mem, entityList, local_player_pawn, cheatData.localPlayer))
        return false;

    cheatData.lookAtEntityIndex = -1;
    const auto lookat_pawn = entityList.getCurrentLookatPlayerPawn(mem, local_player_pawn);

    for (CEntityIndex i = 0; i < 64; i++)
    {
        const auto entity_player_controller = entityList.getPlayerControllerPtr(mem, i);
        const auto entity_player_pawn = entityList.getPawnPtrFromControllerPtr(mem, entity_player_controller);

        const auto success = read_player_data(mem, entityList, entity_player_pawn, cheatData.playerEntities[i]);

        cheatData.playerEntities[i].isValid = success;
    }

    for (CEntityIndex i = 0; i < 64; ++i)
    {
        if (cheatData.playerEntities[i].pawnPtr == lookat_pawn)
            cheatData.lookAtEntityIndex = i;
    }

    read_c4_data(mem, entityList, cheatData.bombData);

    const auto globalVarsPtr = mem.Read<uintptr_t>(client + offsets::globalVars);
    cheatData.globalVars = mem.Read<CGlobalVarsBase>(globalVarsPtr);

    cheatData.viewMatrix = mem.Read<ViewMatrix_t>(client + offsets::ViewMatrix);

    return true;
}
