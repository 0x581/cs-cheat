#pragma once
#include "../memory/memory.h"

#include "cs_defines.h"


struct C4Data {
	bool planted;
	uintptr_t plantedC4Ptr;
	GameTime_t explosionTime;
};

struct PlayerData {
	bool isValid;
	uintptr_t pawnPtr;
	uintptr_t controllerPtr;
	uintptr_t cameraServicesPtr;
	PlayerTeam_t team;
	CHealth maxHealth;
	CHealth health;
	CEntityIndex lookAtEntityIndex;
	Vector3 oldOrigin;
	CBoneArray bones;
	char name[PLAYER_NAME_LENGTH];
	float crouchAmount;
	EntitySpottedState_t spottedByMask;
	bool hasDefuser;
	bool isScoped;
};

struct CheatData {
	PlayerData playerEntities[64]{ 0 };

	PlayerData localPlayer{ 0 };
	CEntityIndex lookAtEntityIndex;

	C4Data bombData{ 0 };

	CGlobalVarsBase globalVars;
	ViewMatrix_t viewMatrix;
};

bool read_cheat_data(Memory& mem, CheatData& cheatData);
