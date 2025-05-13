#pragma once

#include <Windows.h>

#include <chrono>

#include "cs_defines.h"
#include "boneData.h"

struct EspRenderData {
	bool isValid{ false };
	bool isVisible{ false };
	bool isFriendly{ false };
	int health{ 0 };
	float distance{ 0.f };
	char playerName[PLAYER_NAME_LENGTH]{ 0 };
	Vector2 min{ 0, 0 };
	Vector2 max{ 0, 0 };
	BoneData bones{};
	uintptr_t pawnPtr = 0;

};

struct ESPData {
	std::chrono::steady_clock::time_point capture_time{ };
	EspRenderData data[64]{};
};

struct BombData {
	bool planted{ false };
	bool canBeDefused{ false };
	float timeRemaining{ 0.f };
};

extern ESPData espData;
extern BombData bombData;

extern HANDLE consoleOutput;
