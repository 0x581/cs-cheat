

#include <config.h>
#include <keybinds.h>
#include <keyboard.h>
#include <delays.h>

static bool shouldAimBotRun(size_t iteration)
{
	if (iteration % delays::aimbot::ms_interval != delays::aimbot::ms_offset)
		return false;

	if (!config.activeCheats.aimBot)
		return false;

	return IsKeyPressed(keybinds::aimBot);
}

static bool shouldTriggerBotRun(size_t iteration)
{
	if (iteration % delays::triggerBot::ms_interval != delays::triggerBot::ms_offset)
		return false;

	if (!config.activeCheats.triggerBot)
		return false;

	return IsKeyPressed(keybinds::triggerbot);
}

static bool shouldEspRun(size_t iteration)
{
	if (iteration % delays::esp::ms_interval != delays::esp::ms_offset)
		return false;

	return config.activeCheats.esp;
}

static bool shouldBunnyHopRun(size_t iteration)
{
	if (iteration % delays::bunnyHop::ms_interval != delays::bunnyHop::ms_offset)
		return false;

	if (!config.activeCheats.bunnyHop)
		return false;

	return IsKeyPressed(keybinds::bunnyHop);
}

static bool shouldBombTimerRun(size_t iteration)
{
	if (iteration % delays::bombTimer::ms_interval != delays::bombTimer::ms_offset)
		return false;

	return config.activeCheats.bombTimer;
}

static bool shouldAntiFlashRun(size_t iteration)
{
	if (iteration % delays::antiFlash::ms_interval != delays::antiFlash::ms_offset)
		return false;

	return config.activeCheats.antiFlash;
}

static bool shouldFovChangerRun(size_t iteration)
{
	if (iteration % delays::fovChanger::ms_interval != delays::fovChanger::ms_offset)
		return false;

	return config.activeCheats.fovChanger;
}

static bool shouldRadarHackRun(size_t iteration)
{
	if (iteration % delays::radarHack::ms_interval != delays::radarHack::ms_offset)
		return false;

	return config.activeCheats.radar;
}

static bool shouldRageBotRun(size_t iteration)
{
	if (iteration % delays::rageBot::ms_interval != delays::rageBot::ms_offset)
		return false;

	if (!config.activeCheats.rageBot)
		return false;

	return IsKeyPressed(keybinds::rageBot);
}
