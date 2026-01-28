#include "cheat_entries.hpp"

#include "config.hpp"
#include "offsets.hpp"

void runFovChanger(const Memory &mem, const CheatData &cheatData)
{
    const auto isScoped = cheatData.localPlayer.isScoped;

    if (isScoped)
        return;

    auto cameraservices = cheatData.localPlayer.cameraServicesPtr;

    mem.Write<uint32_t>(cameraservices + offsets::fov, config.fov.desiredFov);
}
