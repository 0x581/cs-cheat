#include "../entity_system.h"

#include <offsets.h>
#include <modules.h>
#include <cs_defines.h>

struct EntityList
{
    CEntityIdentity *identity_chunks[MAX_ENTITY_LISTS];
    CEntityIdentity *first_active_entity;
    EntityLinkedList used_list;
    EntityLinkedList dormant_list;
    EntityLinkedList free_networkable_list;
    EntityLinkedList free_non_networkable_list;
    uint32_t networkable_entity_limit;
    uint32_t non_networkable_entity_limit;
    uint32_t max_players;
};

uintptr_t EntityData::getAddressFromIndex(const CEntityIndex index) const
{
    if (index == (CEntityIndex)-1 || index > MAX_TOTAL_ENTITIES)
        return 0;

    const size_t chunk_idx = index >> MAX_ENTITIES_IN_LIST_BITS;
    const size_t list_idx = index & (MAX_ENTITIES_IN_LIST - 1);

    if (chunk_idx >= MAX_ENTITY_LISTS || list_idx >= MAX_ENTITIES_IN_LIST)
    {
        std::cout << "Entity index out of range: Chunk idx " << chunk_idx << ", List idx " << list_idx << "\n";
        return 0;
    }

    if (!identity_chunks_used[chunk_idx])
    {
        std::cout << "Trying to access unused entity chunk: " << chunk_idx << "\n";
        return 0;
    }

    const auto ent_ident = identity_chunks[chunk_idx][list_idx];

    const auto ent_index = ent_ident.get_entity_index();

    if (ent_index == -1)
        return 0; // Entity was simply not valid

    if (ent_index != index)
    {
        std::cout << "Entity index mismatch: Expected " << index << ", got " << ent_index << "\n";
        return 0;
    }

    return ent_ident.instance;
}

inline uintptr_t EntityData::getAddressFromHandle(const CHandle handle) const
{
    return getAddressFromIndex(handle.get_entry_index());
}

uintptr_t EntityData::getPawnPtrFromControllerPtr(const Memory &mem, const uintptr_t controllerPtr) const
{
    if (!controllerPtr)
        return 0;

    const auto handle = mem.Read<CHandle>(controllerPtr + offsets::pawnOfController);

    return getAddressFromHandle(handle);
}

uintptr_t EntityData::getControllerPtrFromPawnPtr(const Memory &mem, const uintptr_t pawnPtr) const
{
    if (!pawnPtr)
        return 0;

    const auto handle = mem.Read<CHandle>(pawnPtr + offsets::controllerOfPawn);

    return getAddressFromHandle(handle);
}

uintptr_t EntityData::getPlayerControllerPtr(const CEntityIndex index) const
{
    if (index >= 64)
        throw std::out_of_range("Player index out of range (0-63)" + std::to_string(index));

    return getAddressFromIndex(index);
}

uintptr_t EntityData::getCurrentLookatPlayerPawn(const Memory &mem, const uintptr_t pawnPtr) const
{
    if (!pawnPtr)
        return 0;

    const auto index = mem.Read<CEntityIndex>(pawnPtr + offsets::idEntIndex);

    return getAddressFromIndex(index);
}

bool read_entity_data(const Memory &mem, EntityData &entityData)
{
    const auto client = mem.GetModuleAddress(modules::client);
    if (!client)
        return false;

    // 0x10 Offset is because the entity list pointer points to the EntitySystem, in which the EntityList is contained
    const auto entity_list_ptr = mem.Read<uintptr_t>(client + offsets::entityList) + 0x10;
    if (!entity_list_ptr)
        return false;

    EntityList entityList;
    auto bytes_read = mem.ReadBytes(entity_list_ptr, reinterpret_cast<byte *>(&entityList), sizeof(EntityList));

    if (bytes_read != sizeof(EntityList))
        return false;

    entityData.identity_chunks_used.reset();

    for (size_t chunk = 0; chunk < MAX_ENTITY_LISTS; ++chunk)
    {
        const uintptr_t chunk_ptr = reinterpret_cast<uintptr_t>(entityList.identity_chunks[chunk]);

        if (!chunk_ptr)
            continue;

        bytes_read = mem.ReadBytes(
            chunk_ptr,
            reinterpret_cast<byte *>(&entityData.identity_chunks[chunk]),
            sizeof(entityData.identity_chunks[0]));

        if (bytes_read != sizeof(entityData.identity_chunks[0]))
            continue;

        entityData.identity_chunks_used.set(chunk, true);
    }

    entityData.first_active_entity = entityList.first_active_entity;
    entityData.used_list = entityList.used_list;
    entityData.max_players = entityList.max_players;

    return true;
}
