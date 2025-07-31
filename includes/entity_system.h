#pragma once
#include <Windows.h>
#include <cstdint>

#include <cs_defines.h>

#include "../src/memory/memory.h"

// Source: https://gist.github.com/a2x/6eb033d3c7b9a97fb7e10d2763de0f9f

static constexpr uint32_t MAX_ENTITIES_IN_LIST = 512;
static constexpr uint32_t MAX_ENTITIES_IN_LIST_BITS = 9;
static constexpr uint32_t MAX_ENTITY_LISTS = 64;
static constexpr uint32_t MAX_TOTAL_ENTITIES = MAX_ENTITIES_IN_LIST * MAX_ENTITY_LISTS;

// Size: 0x78 (120 bytes)
class EntityIdentity
{
public:
    FORCEINLINE int32_t get_entity_index() const noexcept
    {
        return handle.get_entry_index();
    }

public:
    uintptr_t instance; // 0x0000
private:
    uint8_t pad_0008[0x8]; // 0x0008
public:
    CHandle handle; // 0x0010
private:
    uint8_t pad_0014[0x64]; // 0x0014
};

class EntityList
{
public:
    uintptr_t getAddressFromIndex(const Memory &mem, const CEntityIndex index) const;
    uintptr_t getAddressFromHandle(const Memory &mem, const CHandle handle) const;

    uintptr_t getPlayerControllerPtr(const Memory &mem, const CEntityIndex index) const;

    uintptr_t getPawnPtrFromControllerPtr(const Memory &mem, const uintptr_t controllerPtr) const;
    uintptr_t getControllerPtrFromPawnPtr(const Memory &mem, const uintptr_t pawnPtr) const;

    uintptr_t getCurrentLookatPlayerPawn(const Memory &mem, const uintptr_t pawnPtr) const;

private:
    uint8_t pad_0000[0x10];
    EntityIdentity *identity_chunks[MAX_ENTITY_LISTS];
};

bool read_entity_system(const Memory &mem, EntityList &entityList);
