#include "../cheats.h"

#include <vector_math.h>
#include <shared_variables.h>

#include "../../../lib/imgui/pch.h"


static void runEsp(const CheatData& cheatData) {
	const auto localPlayerPawn = cheatData.localPlayer.pawnPtr;
	const auto localPlayerTeam = cheatData.localPlayer.team;

	const auto localPlayerPos = cheatData.localPlayer.oldOrigin;

	const auto view_matrix = cheatData.viewMatrix;

	espData.capture_time = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < 64; i++)
	{
		espData.data[i].isValid = false;

		const auto& playerEntity = cheatData.playerEntities[i];

		const auto playerPawn = playerEntity.pawnPtr;
		const auto controller = playerEntity.controllerPtr;

		if (!playerPawn || playerPawn == localPlayerPawn)
			continue;

		const auto playerTeam = playerEntity.team;

		espData.data[i].isFriendly = localPlayerTeam == playerTeam;

		const auto health = playerEntity.health;

		espData.data[i].maxHealth = playerEntity.maxHealth;
		espData.data[i].health = health;

		if (health <= 0 || health > 100)
			continue;

		memcpy(espData.data[i].playerName, playerEntity.name, PLAYER_NAME_LENGTH);

		espData.data[i].bones = BoneData::fromVector(playerEntity.bones, view_matrix);

		// Esp box
		const auto crouchAmount = playerEntity.crouchAmount;

		Vector3 footPos = playerEntity.oldOrigin;
		Vector3 headPos = { footPos.x, footPos.y, footPos.z + 72.f };

		auto screenFootPos = worldToScreen(footPos, view_matrix);
		auto screenHeadPos = worldToScreen(headPos, view_matrix);

		const auto distance = (localPlayerPos - footPos).length();

		float width = (screenFootPos.y - screenHeadPos.y) / 2.4F;

		if (crouchAmount != 0.f)
		{
			headPos = { footPos.x, footPos.y, footPos.z + 72.f - (crouchAmount * 17.f) };
			screenHeadPos = worldToScreen(headPos, view_matrix);
		}

		espData.data[i].min = { screenHeadPos.x - width / 2,  screenHeadPos.y };
		espData.data[i].max = { screenHeadPos.x + width / 2, screenFootPos.y };
		espData.data[i].distance = distance;

		espData.data[i].isValid = true;
	}
}
