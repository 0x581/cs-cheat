#include "cheat_entries.h"

#include <config.h>
#include <offsets.h>

void runFovChanger(const Memory &mem, const CheatData &cheatData)
{
    const auto isScoped = cheatData.localPlayer.isScoped;

    if (isScoped)
        return;

    auto cameraservices = cheatData.localPlayer.cameraServicesPtr;

    mem.Write<uint32_t>(cameraservices + offsets::FOV, config.fov.desiredFov);
}
