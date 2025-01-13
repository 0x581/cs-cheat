#pragma once

#include "../memory.hpp"
#include "../offsets/offsets.hpp"
#include "../defines.hpp"

const uintptr_t getEntityListPtr(const Memory& mem)
{
    const auto client = mem.GetModuleAddress(modules::client);

    return mem.Read<uintptr_t>(client + offsets::entityList);
}


uintptr_t getControllerPtr(const Memory& mem, uintptr_t index)
{
    if (index >= 64)
        exit(-1);

    const auto entityList = getEntityListPtr(mem);

    if (!entityList)
        return 0;

    const auto controllerList = mem.Read<uintptr_t>(entityList + 0x10);

    if (!controllerList)
        return 0;

    // The index is multiplied by 0x78 as that is the distance from one Controller Ptr to the next
    // https://youtu.be/nB9metCiG7A?si=r5Qlorcxqdzs0Wjg&t=870
    return mem.Read<uintptr_t>(controllerList + index * 0x78);
}

uintptr_t getObjectPtrFromHandle(const Memory& mem, uintptr_t handle)
{
    if (handle == 0)
        return 0;

    const auto entityListPtr = getEntityListPtr(mem);

    // The handle's 15 lower bits are used to get its pointer.
    // The higher 6 bits are used to get the index into the entity lists
    const auto entityListIndex = (handle & 0x7FFF) >> 9;

    // The lower 9 bits are used to get the index into the sub list
    const auto subListIndex = handle & 0x1FF;

    // Multiply by 8 because pointers have a size of 8 bytes
    const auto subList = mem.Read<uintptr_t>(entityListPtr + 0x10 + 0x8 * entityListIndex);

    // The index is multiplied by 0x78 as that is the distance from one Ptr to the next
    return mem.Read<uintptr_t>(subList + 0x78 * subListIndex);
}

uintptr_t getPawnPtrFromControllerPtr(const Memory& mem, uintptr_t controllerPtr)
{
    if (!controllerPtr)
        return 0;

    const auto pawnHandle = mem.Read<uintptr_t>(controllerPtr + offsets::pawnOfController);

    return getObjectPtrFromHandle(mem, pawnHandle);
}

uintptr_t getControllerPtrFromPawnPtr(const Memory& mem, uintptr_t pawnPtr)
{
    if (!pawnPtr)
        return 0;

    const auto controllerHandle = mem.Read<uintptr_t>(pawnPtr + offsets::controllerOfPawn);

    return getObjectPtrFromHandle(mem, controllerHandle);
}


uintptr_t getCurrentLookatEnemyPawn(const Memory& mem)
{
    const auto client = mem.GetModuleAddress(modules::client);

    const auto localPlayer = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);

    const auto entIndex = mem.Read<CEntityIndex>(localPlayer + offsets::IDEntIndex);

    if (entIndex == (unsigned int)-1)
        return 0;

    // Get the local player's team and entity index
    const auto team = mem.Read<uint8_t>(localPlayer + offsets::teamNum);

    const auto entityPawn = getObjectPtrFromHandle(mem, entIndex);

    // Get the enemy's team (target entity)
    const auto otherTeam = mem.Read<uint8_t>(entityPawn + offsets::teamNum);

    // Check if the target is an enemy (i.e. not on the same team)
    if (team != otherTeam)
        return entityPawn;

    // TODO: Add check for Player

    return 0;
}

uintptr_t getCurrentLookatEnemyController(const Memory& mem)
{
    const auto pawn = getCurrentLookatEnemyPawn(mem);
    return getControllerPtrFromPawnPtr(mem, pawn);
}


uintptr_t getCurrentLookatPlayerPawn(const Memory& mem)
{
    const auto client = mem.GetModuleAddress(modules::client);

    const auto localPlayer = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);

    const auto entIndex = mem.Read<CEntityIndex>(localPlayer + offsets::IDEntIndex);

    if (entIndex == (unsigned int)-1)
        return 0;

    // Get the local player's team and entity index
    const auto team = mem.Read<uint8_t>(localPlayer + offsets::teamNum);

    // TODO: Add check for Player

    return getObjectPtrFromHandle(mem, entIndex);
}

uintptr_t getCurrentLookatPlayerController(const Memory& mem)
{
    const auto pawn = getCurrentLookatPlayerPawn(mem);
    return getControllerPtrFromPawnPtr(mem, pawn);
}
