#include "cheat_entries.h"

#include <cmath>

#include <cs_defines.h>
#include <vector_math.h>
#include <config.h>


void runAimBot(const CheatData& cheat_data) {
	Vector2 screenCenter{ (float)GetSystemMetrics(SM_CXSCREEN) / 2.f, (float)GetSystemMetrics(SM_CYSCREEN) / 2.f };

	const auto localPlayerPawn = cheat_data.localPlayer.pawnPtr;
	const auto localPlayerController = cheat_data.localPlayer.controllerPtr;
	const auto localPlayerTeam = cheat_data.localPlayer.team;

	auto lowestDistance = HUGE_VAL;
	ImVec2 closestTargetScreen{ 0, 0 };

	const auto localPawnIndex = cheat_data.localPlayer.lookAtEntityIndex;

	for (CEntityIndex i = 0; i < 64; i++)
	{
		const auto& playerEntity = cheat_data.playerEntities[i];

		if (!playerEntity.isValid)
			continue;

		const auto playerPawn = playerEntity.pawnPtr;

		if (!playerPawn || playerPawn == localPlayerPawn)
			continue;

		const auto playerTeam = playerEntity.team;

		if (config.aimBot.ignoreTeam && (localPlayerTeam == playerTeam))
			continue;

		const auto health = playerEntity.health;
		if (health <= 0 || health > 100)
			continue;


		// --- Hitbox-Auswahl und Sichtbarkeitsprüfung ---
		CBoneData targetBone = { 0 };
		bool isVisible = false;

		switch (config.aimBot.hitbox)
		{
		case 0:
			targetBone = playerEntity.bones.head;
			break;
		case 1:
			targetBone = playerEntity.bones.neck;
			break;
		case 2:
			targetBone = playerEntity.bones.spine1;
			break;
		default:
			targetBone = playerEntity.bones.head;
			break;
		}

		if (targetBone.location == Vector3(0, 0, 0))
			continue;

		const auto spottedByMask = playerEntity.spottedByMask;

		isVisible = (spottedByMask & (1ULL << localPawnIndex)) != 0;

		if (config.aimBot.checkVisible && !isVisible)
			continue;

		// --- Ende Hitbox/Sichtbarkeit ---
		const auto screen_location = worldToScreen(targetBone.location, cheat_data.viewMatrix);

		double distance = std::pow(screen_location.x - screenCenter.x, 2) + std::pow(screen_location.y - screenCenter.y, 2);

		if (distance >= lowestDistance)
			continue;

		lowestDistance = distance;
		closestTargetScreen = screen_location.xy();
	}



	if (lowestDistance == HUGE_VAL)
		return;

	const auto radius = config.aimBot.pixelRadius;
	const auto maxDistance = radius * radius;

	if (lowestDistance > maxDistance)
		return;

	const auto diff_x = closestTargetScreen.x - screenCenter.x;
	const auto diff_y = closestTargetScreen.y - screenCenter.y;

	const auto speedFactor = config.aimBot.speedFactor;

	if (speedFactor <= 0.001f) return;

	long new_x = static_cast<long>(diff_x * speedFactor);
	long new_y = static_cast<long>(diff_y * speedFactor);

	if (new_x != 0 || new_y != 0)
	{
		mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(new_x), static_cast<DWORD>(new_y), 0, 0);
	}
}
