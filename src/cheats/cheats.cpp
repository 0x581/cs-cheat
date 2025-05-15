
#include <config.h>
#include <thread>

#include "delays.h"

#include "check_cheat_should_run.cpp"

#include "src/aimBot.cpp"
#include "src/esp.cpp"
#include "src/bunnyHop.cpp"
#include "src/radarHack.cpp"
#include "src/triggerBot.cpp"
#include "src/bombTimer.cpp"
#include "src/antiFlash.cpp"
#include "src/fovChanger.cpp"


void run_all_cheats(Memory& mem, CheatData& cheat_data, size_t iteration) {
	if (shouldAimBotRun(iteration))
		runAimBot(cheat_data);

	if (shouldTriggerBotRun(iteration))
		runTriggerBot(cheat_data);

	if (shouldEspRun(iteration))
		runEsp(cheat_data);

	if (shouldBunnyHopRun(iteration))
		runBunnyHop();

	if (shouldBombTimerRun(iteration))
		runbombTimer(cheat_data);

	if (shouldAntiFlashRun(iteration))
		runAntiFlash(mem, cheat_data);

	if (shouldFovChangerRun(iteration))
		runFovChanger(mem, cheat_data);

	if (shouldRadarHackRun(iteration))
		runRadarHack(mem, cheat_data);
}

void run_all_cheats_loop(Memory& mem) {
	CheatData cheat_data{ 0 };

	std::chrono::time_point<std::chrono::high_resolution_clock> lastTime = std::chrono::high_resolution_clock::now();

	for (size_t iteration = 0;; ++iteration)
	{
		lastTime += std::chrono::milliseconds(1);

		std::this_thread::sleep_until(lastTime);

		if (!mem.is_valid)
		{
			std::this_thread::sleep_for(delays::memConnectionWaitTime);
			continue;
		}

		if (!read_cheat_data(mem, cheat_data))
			continue;

		run_all_cheats(mem, cheat_data, iteration);
	}
}