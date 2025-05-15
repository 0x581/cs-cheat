#pragma once

#include <array>

#include "vector_math.h"

typedef int CHealth;

typedef unsigned long long CEntityIndex;
typedef uint8_t PlayerTeam_t;
typedef unsigned long long EntitySpottedState_t;
typedef float GameTime_t;

class CHandle {
public:
	[[nodiscard]] std::uint32_t get_entry_index() const noexcept {
		return index_ & 0x7FFF;
	}

	[[nodiscard]] bool is_valid() const noexcept {
		return index_ > 0 && index_ != 0xFFFFFFFF;
	}

	[[nodiscard]] bool operator==(const CHandle& other) const noexcept {
		return index_ == other.index_;
	}

	[[nodiscard]] bool operator!=(const CHandle& other) const noexcept {
		return index_ != other.index_;
	}

private:
	uint32_t index_;
};

struct Quaternion { float x, y, z, w; };
struct CBoneData {
	Vector3 location{ 0 };
	float scale{ 0 };
	Quaternion rotation{ 0 };
};

struct CBoneArray {
	CBoneData hip;			// 0
	CBoneData spine1;		// 1
	CBoneData spine2;		// 2
	CBoneData spine3;		// 3
	CBoneData spine4;		// 4
	CBoneData neck;			// 5
	CBoneData head;			// 6

	CBoneData unknown1;		// 7

	CBoneData l_shoulder;	// 8
	CBoneData l_arm;		// 9
	CBoneData l_wrist;		// 10
	CBoneData l_hand;		// 11

	CBoneData unknown2;		// 12

	CBoneData r_shoulder;	// 13
	CBoneData r_arm;		// 14
	CBoneData r_wrist;		// 15
	CBoneData r_hand;		// 16

	CBoneData unknown3;		// 17
	CBoneData unknown4;		// 18
	CBoneData unknown5;		// 19
	CBoneData unknown6;		// 20
	CBoneData unknown7;		// 21

	CBoneData l_hip;		// 22
	CBoneData l_knee;		// 23
	CBoneData l_foot;		// 24

	CBoneData r_hip;		// 25
	CBoneData r_knee;		// 26
	CBoneData r_foot;		// 27

	CBoneData unknown8;		// 28
	CBoneData unknown9;		// 29
};

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
