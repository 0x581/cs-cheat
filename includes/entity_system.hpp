#pragma once

#include <bitset>
#include <cstdint>

#include <Windows.h>

#include "CheatBase/memory.hpp"

#include "cs_defines.hpp"

// Source: https://gist.github.com/a2x/6eb033d3c7b9a97fb7e10d2763de0f9f

static constexpr uint32_t MAX_ENTITIES_IN_LIST = 512;
static constexpr uint32_t MAX_ENTITIES_IN_LIST_BITS = 9;
static constexpr uint32_t MAX_ENTITY_LISTS = 64;
static constexpr uint32_t MAX_TOTAL_ENTITIES = MAX_ENTITIES_IN_LIST * MAX_ENTITY_LISTS;

typedef uintptr_t CEntityIdentityPtr;
typedef uintptr_t CSCharPtr;

// Size: 0x70 (112 bytes)
class CEntityIdentity
{
public:
    FORCEINLINE int32_t get_entity_index() const noexcept
    {
        return handle.get_entry_index();
    }

public:
    uintptr_t instance;

private:
    uint8_t pad_0008[0x8];

public:
    CHandle handle;

private:
    int32_t m_nameStringableIndex;
    CSCharPtr m_name;
    CSCharPtr m_designerName;
    // uint8_t pad_0028[0x8];
    // uint32_t m_flags;
    // uint32_t m_worldGroupId;
    // uint32_t m_fDataObjectTypes;
    // uint8_t pad_0040[0x18];
    // CEntityIdentityPtr m_pPrev;
    // CEntityIdentityPtr m_pNext;
    // CEntityIdentityPtr m_pPrevByClass;
    // CEntityIdentityPtr m_pNextByClass;
    uint8_t pad_0028[0x44];
};

static_assert(sizeof(CEntityIdentity) == 0x70, "CEntityIdentity size mismatch");

struct EntityLinkedList
{
    CEntityIdentity *head;
    CEntityIdentity *tail;
    uint8_t pad_0010[0x8];
};

class EntityData
{
public:
    uintptr_t getAddressFromIndex(const CEntityIndex index) const;
    inline uintptr_t getAddressFromHandle(const CHandle handle) const;

    uintptr_t getPlayerControllerPtr(const CEntityIndex index) const;

    uintptr_t getPawnPtrFromControllerPtr(const Memory &mem, const uintptr_t controllerPtr) const;
    uintptr_t getControllerPtrFromPawnPtr(const Memory &mem, const uintptr_t pawnPtr) const;

    uintptr_t getCurrentLookatPlayerPawn(const Memory &mem, const uintptr_t pawnPtr) const;

    std::bitset<MAX_ENTITY_LISTS> identity_chunks_used;
    CEntityIdentity identity_chunks[MAX_ENTITY_LISTS][MAX_ENTITIES_IN_LIST];

    CEntityIdentity *first_active_entity;
    EntityLinkedList used_list;

    uint32_t max_players;
};

bool read_entity_data(const Memory &mem, EntityData &entityData);
