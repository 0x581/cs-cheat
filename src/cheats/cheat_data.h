#pragma once
#include "../memory/memory.h"

#include "entity_system.h"
#include <cs_defines.h>


struct C4Data {
	bool planted;
	uint32_t site;
	GameTime_t explosionTime;
	bool beingDefused;
	bool defused;
	CEntityIndex defuser;
};

struct PlayerData {
	bool isValid{ 0 };
	uintptr_t pawnPtr{ 0 };
	uintptr_t controllerPtr{ 0 };
	uintptr_t cameraServicesPtr{ 0 };
	PlayerTeam_t team{ 0 };
	CHealth maxHealth{ 0 };
	CHealth health{ 0 };
	CEntityIndex lookAtEntityIndex{ 0 };
	Vector3 oldOrigin{ 0 };
	CBoneArray bones{ 0 };
	char name[PLAYER_NAME_LENGTH]{ 0 };
	float crouchAmount{ 0 };
	EntitySpottedState_t spottedByMask{ 0 };
	bool hasDefuser{ 0 };
	bool isScoped{ 0 };
};

struct CheatData {
	PlayerData playerEntities[64];

	PlayerData localPlayer;
	CEntityIndex lookAtEntityIndex{ 0 };

	C4Data bombData;

	CGlobalVarsBase globalVars;
	ViewMatrix_t viewMatrix{ 0 };
};

bool read_cheat_data(const Memory& mem, const EntityList& entityList, CheatData& cheatData);
