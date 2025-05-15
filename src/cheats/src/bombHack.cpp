#include "../cheats.h"

#include <shared_variables.h>


static void runBombHack(const CheatData& cheatData) {
	if (!cheatData.bombData.planted)
	{
		bombData.planted = false;
		return;
	}

	const auto explosionTime = cheatData.bombData.explosionTime;
	const auto currentTime = cheatData.globalVars.currentTime;

	const auto timeRemaining = explosionTime - currentTime;

	const auto hasDefuser = cheatData.localPlayer.hasDefuser;

	const auto defuseTime = hasDefuser ? 5.0 : 10.0;

	bombData.canBeDefused = timeRemaining > defuseTime;
	bombData.timeRemaining = timeRemaining;
	bombData.planted = true;
}
