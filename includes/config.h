#pragma once

#include "../lib/imgui/pch.h"

#define MAGIC_BYTES "Merkur CS2 Config"

constexpr auto COLOR_NAME_MAX_LENGTH = 64;


struct PersistentConfigData {
	const char _magicBytes[sizeof(MAGIC_BYTES)] = MAGIC_BYTES;

	struct MainWindowSettings {
		ImVec2 windowPosition;
	} mainWindow;

	struct BombSettings {
		ImVec2 windowPosition;
	} bombWindow;
};

struct ConfigData {
	const char _magicBytes[sizeof(MAGIC_BYTES)] = MAGIC_BYTES;

	struct ActiveCheats {
		bool aimBot;
		bool antiFlash;
		bool bombTimer;
		bool bunnyHop;
		bool esp;
		bool fovChanger;
		bool radar;
		bool triggerBot;
		bool rageBot;
	} activeCheats;

	struct EspSettings {
		char selectedColor[COLOR_NAME_MAX_LENGTH];
		bool renderFriendlies;
		bool boxGlow;
		bool bone;
		bool box;
		bool name;
		bool health;

		ImVec4 colorEnemyVisible;
		ImVec4 colorEnemyHidden;
		ImVec4 colorTeamVisible;
		ImVec4 colorTeamHidden;
		ImVec4 colorItem;
	}esp;

	struct AimBotSettings {
		float pixelRadius;
		float speedFactor;
		bool drawFovCircle;
		bool ignoreTeam;
		bool checkVisible;
		ImVec4 fovCircleColor;
		int hitbox;
		int shotDelay = 30;
	} aimBot;

	struct FovSettings {
		int desiredFov;
	} fov;

	struct RageBotSettings {
		float pixelRadius = 120.f;
		float speedFactor = 1.0f;
		bool visible = false;
		int hitbox = 0;
		int shotDelay = 30; // ms
	} rageBot;
};


extern ConfigData config;
extern PersistentConfigData persistentConfig;

bool load_cheat_config_data(ConfigData* data, const wchar_t* folder, const wchar_t* file);
bool load_persistent_config_data(PersistentConfigData* data, const wchar_t* folder, const wchar_t* file);

bool store_cheat_config_data(const ConfigData data, const wchar_t* folder, const wchar_t* file);
bool store_persistent_config_data(const PersistentConfigData data, const wchar_t* folder, const wchar_t* file);
