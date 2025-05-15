#include "../cheats.h"

#include <windows.h>

#include <config.h>

static void runTriggerBot(const CheatData& cheatData) {

	const auto lookAtEntityIdx = cheatData.lookAtEntityIndex;

	if (lookAtEntityIdx == -1)
		return;

	const auto lookAtEntity = cheatData.playerEntities[lookAtEntityIdx];

	if (!lookAtEntity.isValid)
		return;

	const auto targetHealth = lookAtEntity.health;

	if (targetHealth <= 0 || targetHealth > 100)
		return;

	const auto localPlayerTeam = cheatData.localPlayer.team;
	const auto targetTeam = lookAtEntity.team;

	if (config.aimBot.ignoreTeam && (localPlayerTeam == targetTeam))
		return;

	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}
