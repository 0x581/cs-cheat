#include "../entity_system.h"

#include <offsets.h>
#include <modules.h>
#include <cs_defines.h>

uintptr_t EntityList::getAddressFromIndex(const Memory &mem, const CEntityIndex index) const
{
    if (index == (CEntityIndex)-1 || index > MAX_TOTAL_ENTITIES)
        return 0;

    const auto chunk_idx = index >> MAX_ENTITIES_IN_LIST_BITS;
    const auto chunk_to_use = identity_chunks[chunk_idx];

    if (chunk_to_use == nullptr)
        return 0;

    const auto list_idx = index & (MAX_ENTITIES_IN_LIST - 1);
    const auto entity_identity_address = reinterpret_cast<uintptr_t>(chunk_to_use) + (list_idx * sizeof(EntityIdentity));

    const auto ent_ident = mem.Read<EntityIdentity>(entity_identity_address);

    if (ent_ident.get_entity_index() != index)
        return 0;

    return ent_ident.instance;
}

uintptr_t EntityList::getAddressFromHandle(const Memory &mem, const CHandle handle) const
{
    return getAddressFromIndex(mem, handle.get_entry_index());
}

uintptr_t EntityList::getPawnPtrFromControllerPtr(const Memory &mem, const uintptr_t controllerPtr) const
{
    if (!controllerPtr)
        return 0;

    const auto handle = mem.Read<CHandle>(controllerPtr + offsets::pawnOfController);

    return getAddressFromHandle(mem, handle);
}

uintptr_t EntityList::getControllerPtrFromPawnPtr(const Memory &mem, const uintptr_t pawnPtr) const
{
    if (!pawnPtr)
        return 0;

    const auto handle = mem.Read<CHandle>(pawnPtr + offsets::controllerOfPawn);

    return getAddressFromHandle(mem, handle);
}

uintptr_t EntityList::getPlayerControllerPtr(const Memory &mem, const CEntityIndex index) const
{
    if (index >= 64)
        throw std::out_of_range("Player index out of range (0-63)" + std::to_string(index));

    return getAddressFromIndex(mem, index);
}

uintptr_t EntityList::getCurrentLookatPlayerPawn(const Memory &mem, const uintptr_t pawnPtr) const
{
    if (!pawnPtr)
        return 0;

    const auto index = mem.Read<CEntityIndex>(pawnPtr + offsets::idEntIndex);

    return getAddressFromIndex(mem, index);
}

bool read_entity_system(const Memory &mem, EntityList &entityList)
{
    const auto client = mem.GetModuleAddress(modules::client);
    if (!client)
        return false;

    const auto entity_list_ptr = mem.Read<uintptr_t>(client + offsets::entityList);
    if (!entity_list_ptr)
        return false;

    const auto bytes_read = mem.ReadBytes(entity_list_ptr, reinterpret_cast<byte *>(&entityList), sizeof(EntityList));

    return bytes_read == sizeof(EntityList);
}
