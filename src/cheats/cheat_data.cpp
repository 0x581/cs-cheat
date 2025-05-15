#include "cheat_data.h"

#include <modules.h>
#include <offsets.h>
#include <entityListInteraction.h>
#include <strings.h>
#include <vector_math.h>

void read_c4_data(Memory& mem, C4Data& c4Data) {
	const auto client = mem.GetModuleAddress(modules::client);

	if (!client)
		return;

	c4Data.planted = mem.Read<bool>(client + offsets::plantedC4 - 0x8);

	if (!c4Data.planted)
		return;

	const auto plantedC4Address = mem.Read<uintptr_t>(client + offsets::plantedC4);
	c4Data.plantedC4Ptr = mem.Read<uintptr_t>(plantedC4Address);

	c4Data.explosionTime = mem.Read<GameTime_t>(c4Data.plantedC4Ptr + offsets::c4ExplosionTime);
}

bool read_player_data(Memory& mem, uintptr_t playerPawnPtr, PlayerData& playerData) {
	if (!playerPawnPtr)
		return false;

	playerData.pawnPtr = playerPawnPtr;

	const auto controllerPtr = getControllerPtrFromPawnPtr(mem, playerPawnPtr);
	if (!controllerPtr)
		return false;

	playerData.controllerPtr = controllerPtr;
	playerData.team = mem.Read<PlayerTeam_t>(playerPawnPtr + offsets::teamNum);
	playerData.health = mem.Read<CHealth>(playerPawnPtr + offsets::health);
	playerData.maxHealth = mem.Read<CHealth>(playerPawnPtr + offsets::maxHealth);
	playerData.lookAtEntityIndex = mem.Read<CEntityIndex>(playerPawnPtr + offsets::IDEntIndex);

	memset(playerData.name, 0, PLAYER_NAME_LENGTH);
	mem.ReadBytes(controllerPtr + offsets::playerName, (byte*)playerData.name, PLAYER_NAME_LENGTH);

	parseString(playerData.name, playerData.name, PLAYER_NAME_LENGTH);

	playerData.oldOrigin = mem.Read<Vector3>(playerPawnPtr + offsets::OldOrigin);

	const auto skeletonInstance = mem.Read<uintptr_t>(playerPawnPtr + offsets::gameSceneNode);
	const auto boneArrayPtr = mem.Read<uintptr_t>(skeletonInstance + offsets::modelState + 0x80);
	playerData.bones = mem.Read<CBoneArray>(boneArrayPtr);

	const auto movementServices = mem.Read<intptr_t>(playerPawnPtr + offsets::movementServices);
	playerData.crouchAmount = mem.Read<float>(movementServices + offsets::crouchAmount);

	playerData.spottedByMask = mem.Read<EntitySpottedState_t>(playerPawnPtr + offsets::entitySpottedState + offsets::spotted);

	const auto itemServices = mem.Read<uintptr_t>(playerPawnPtr + offsets::itemServices);
	playerData.hasDefuser = mem.Read<bool>(itemServices + offsets::hasDefuser);

	playerData.isScoped = mem.Read<bool>(playerPawnPtr + offsets::IsScoped);

	playerData.cameraServicesPtr = mem.Read<uintptr_t>(playerPawnPtr + offsets::CameraServices);

	return true;
}

bool read_cheat_data(Memory& mem, CheatData& cheatData) {
	memset(&cheatData, 0, sizeof(CheatData));

	const auto client = mem.GetModuleAddress(modules::client);

	if (!client)
		return false;

	const auto localPlayerPawn = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);

	if (!localPlayerPawn)
		return false;

	if (!read_player_data(mem, localPlayerPawn, cheatData.localPlayer))
		return false;

	for (CEntityIndex i = 0; i < 64; i++)
	{
		const auto entityPlayerPawn = getPlayerPawnPtr(mem, i);

		const auto success = read_player_data(mem, entityPlayerPawn, cheatData.playerEntities[i]);

		cheatData.playerEntities[i].isValid = success;
	}

	cheatData.lookAtEntityIndex = -1;
	const auto lookAtPawn = getCurrentLookatPlayerPawn(mem);

	for (CEntityIndex i = 0; i < 64; ++i)
	{
		if (cheatData.playerEntities[i].pawnPtr == lookAtPawn)
			cheatData.lookAtEntityIndex = i;
	}

	read_c4_data(mem, cheatData.bombData);

	const auto globalVarsPtr = mem.Read<uintptr_t>(client + offsets::globalVars);
	cheatData.globalVars = mem.Read<CGlobalVarsBase>(globalVarsPtr);

	cheatData.viewMatrix = mem.Read<ViewMatrix_t>(client + offsets::ViewMatrix);

	return true;
}
