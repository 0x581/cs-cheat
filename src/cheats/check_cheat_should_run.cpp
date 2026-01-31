#include "cheats/cheats.hpp"

#include <format>
#include <iostream>

#include "CheatBase/keyboard.hpp"

#include "config.hpp"
#include "delays.hpp"

static bool shouldCheatRun(bool active, int ms_interval, int ms_offset, size_t iteration)
{
    if (!active)
        return false;

    if (iteration % ms_interval != ms_offset)
        return false;

    return true;
}

static bool shouldCheatRun(bool active, int ms_interval, int ms_offset, size_t iteration, ToggleableKeybind &keybind)
{
    if (!shouldCheatRun(active, ms_interval, ms_offset, iteration))
        return false;

    return keybind.get();
}

bool shouldAimBotRun(size_t iteration)
{
    if (iteration % delays::aimbot::ms_interval != delays::aimbot::ms_offset)
        return false;

    if (!cfg.cheat.activeCheats.aimBot)
        return false;

    return cfg.cheat.keybinds.aimBot.get();
}

bool shouldTriggerBotRun(size_t iteration)
{
    if (iteration % delays::triggerBot::ms_interval != delays::triggerBot::ms_offset)
        return false;

    if (!cfg.cheat.activeCheats.triggerBot)
        return false;

    return cfg.cheat.keybinds.triggerBot.get();
}

bool shouldEspRun(size_t iteration)
{
    if (iteration % delays::esp::ms_interval != delays::esp::ms_offset)
        return false;

    return cfg.cheat.activeCheats.esp;
}

bool shouldBunnyHopRun(size_t iteration)
{
    if (iteration % delays::bunnyHop::ms_interval != delays::bunnyHop::ms_offset)
        return false;

    if (!cfg.cheat.activeCheats.bunnyHop)
        return false;

    return cfg.cheat.keybinds.bunnyHop.get();
}

bool shouldBombTimerRun(size_t iteration)
{
    if (iteration % delays::bombTimer::ms_interval != delays::bombTimer::ms_offset)
        return false;

    return cfg.cheat.activeCheats.bombTimer;
}

bool shouldAntiFlashRun(size_t iteration)
{
    if (iteration % delays::antiFlash::ms_interval != delays::antiFlash::ms_offset)
        return false;

    return cfg.cheat.activeCheats.antiFlash;
}

bool shouldFovChangerRun(size_t iteration)
{
    if (iteration % delays::fovChanger::ms_interval != delays::fovChanger::ms_offset)
        return false;

    return cfg.cheat.activeCheats.fovChanger;
}

bool shouldRadarHackRun(size_t iteration)
{
    if (iteration % delays::radarHack::ms_interval != delays::radarHack::ms_offset)
        return false;

    return cfg.cheat.activeCheats.radar;
}

bool shouldRageBotRun(size_t iteration)
{
    if (iteration % delays::rageBot::ms_interval != delays::rageBot::ms_offset)
        return false;

    if (!cfg.cheat.activeCheats.rageBot)
        return false;

    return cfg.cheat.keybinds.rageBot.get();
}
