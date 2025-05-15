

#include <config.h>
#include <keybinds.h>

static bool shouldAimBotRun(size_t iteration) {
	if (iteration % 15 != 0)
		return false;

	if (!config.activeCheats.aimBot)
		return false;

	return GetAsyncKeyState(keybinds::aimBot);
}

static bool shouldTriggerBotRun(size_t iteration) {
	if (iteration % 15 != 5)
		return false;

	if (!config.activeCheats.triggerBot)
		return false;

	return GetAsyncKeyState(keybinds::triggerbot);
}

static bool shouldEspRun(size_t iteration) {
	if (iteration % 5 != 0)
		return false;

	return config.activeCheats.esp;
}

static bool shouldBunnyHopRun(size_t iteration) {
	if (iteration % 15 != 0)
		return false;

	if (!config.activeCheats.bunnyHop)
		return false;

	return GetAsyncKeyState(keybinds::bunnyHop);
}

static bool shouldBombHackRun(size_t iteration) {
	if (iteration % 50 != 0)
		return false;

	return config.activeCheats.bombHack;
}

static bool shouldAntiFlashRun(size_t iteration) {
	if (iteration % 10 != 0)
		return false;

	return config.activeCheats.antiFlash;
}

static bool shouldFovChangerRun(size_t iteration) {
	if (iteration % 5 != 0)
		return false;

	return config.activeCheats.fovChanger;
}

static bool shouldRadarHackRun(size_t iteration) {
	if (iteration % 1000 != 0)
		return false;

	return config.activeCheats.radar;
}