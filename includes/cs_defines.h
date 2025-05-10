#pragma once

#include <array>

#include "vector_math.h"

typedef unsigned int CEntityIndex;
typedef float GameTime_t;
typedef uint8_t PlayerTeam_t;

struct Quaternion { float x, y, z, w; };
struct CBoneData {
	Vector3 location;
	float scale;
	Quaternion rotation;
};

typedef std::array<CBoneData, 30> CBoneArray;

class CGlobalVarsBase {
public:
	float realTime;
	int frameCount;
	float absoluteFrameTime;
	float absoluteFrameStartTimeStdDev;
	int maxClients;
	float intervalPerTick;
private:
	char pad1[0x1C];
public:
	float currentTime;
	float currentTime2;
private:
	char pad2[0xC];
public:
	int32_t tickCount;
	float intervalPerTick2;
private:
	char pad3[0x138];
public:
	uint64_t currentMap;
	uint64_t currentMapName;
};
// Don't trust this struct. I had to manually fix some parts of it already. It should be fine until currentTime2 tho...

constexpr auto PLAYER_NAME_LENGTH = 128;
