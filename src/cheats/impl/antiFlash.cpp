#include "cheat_entries.hpp"

#include "offsets.hpp"

void runAntiFlash(const Memory &mem, const CheatData &cheatData)
{
    const auto localPlayer = cheatData.localPlayer.pawnPtr;

    mem.Write<float>(localPlayer + offsets::flashDuration, 0);
}
