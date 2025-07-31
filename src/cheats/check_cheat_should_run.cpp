

#include <config.h>
#include <keyboard.h>
#include <delays.h>

static bool handleToggleableKeybind(ToggleableKeybind &keybind, bool &key_pressed_last_frame)
{
    const bool isKeyPressed = IsKeyPressed(keybind.key);

    if (keybind.keybindChanged)
    {
        key_pressed_last_frame = true;

        if (!isKeyPressed)
            keybind.keybindChanged = false;

        return false;
    }

    if (keybind.isToggle && (isKeyPressed && !key_pressed_last_frame))
    {
        keybind.toggleState = !keybind.toggleState;
        std::cout << "Toggle state changed to : " << (keybind.toggleState ? "ON" : "OFF") << std::endl;
    }

    key_pressed_last_frame = isKeyPressed;

    return keybind.isToggle ? keybind.toggleState : isKeyPressed;
}

bool shouldAimBotRun(size_t iteration)
{
    if (iteration % delays::aimbot::ms_interval != delays::aimbot::ms_offset)
        return false;

    if (!config.activeCheats.aimBot)
        return false;

    static bool key_pressed_last_frame = false;
    return handleToggleableKeybind(config.keybinds.aimBot, key_pressed_last_frame);
}

bool shouldTriggerBotRun(size_t iteration)
{
    if (iteration % delays::triggerBot::ms_interval != delays::triggerBot::ms_offset)
        return false;

    if (!config.activeCheats.triggerBot)
        return false;

    static bool key_pressed_last_frame = false;
    return handleToggleableKeybind(config.keybinds.triggerBot, key_pressed_last_frame);
}

bool shouldEspRun(size_t iteration)
{
    if (iteration % delays::esp::ms_interval != delays::esp::ms_offset)
        return false;

    return config.activeCheats.esp;
}

bool shouldBunnyHopRun(size_t iteration)
{
    if (iteration % delays::bunnyHop::ms_interval != delays::bunnyHop::ms_offset)
        return false;

    if (!config.activeCheats.bunnyHop)
        return false;

    static bool key_pressed_last_frame = false;
    return handleToggleableKeybind(config.keybinds.bunnyHop, key_pressed_last_frame);
}

bool shouldBombTimerRun(size_t iteration)
{
    if (iteration % delays::bombTimer::ms_interval != delays::bombTimer::ms_offset)
        return false;

    return config.activeCheats.bombTimer;
}

bool shouldAntiFlashRun(size_t iteration)
{
    if (iteration % delays::antiFlash::ms_interval != delays::antiFlash::ms_offset)
        return false;

    return config.activeCheats.antiFlash;
}

bool shouldFovChangerRun(size_t iteration)
{
    if (iteration % delays::fovChanger::ms_interval != delays::fovChanger::ms_offset)
        return false;

    return config.activeCheats.fovChanger;
}

bool shouldRadarHackRun(size_t iteration)
{
    if (iteration % delays::radarHack::ms_interval != delays::radarHack::ms_offset)
        return false;

    return config.activeCheats.radar;
}

bool shouldRageBotRun(size_t iteration)
{
    if (iteration % delays::rageBot::ms_interval != delays::rageBot::ms_offset)
        return false;

    if (!config.activeCheats.rageBot)
        return false;

    static bool key_pressed_last_frame = false;
    return handleToggleableKeybind(config.keybinds.rageBot, key_pressed_last_frame);
}
