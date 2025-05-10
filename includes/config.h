#pragma once

#include <iostream>
#include <filesystem>

#include "../lib/pch.h"

using std::filesystem::path;

constexpr char MAGIC_BYTES[64]{ "Merkur CS2 Config" };
constexpr auto configFolder = "Merkur/CS2/"; // In Local Appdata

constexpr auto COLOR_NAME_MAX_LENGTH = 64;


struct PersistentConfigData {
	struct MainWindowSettings {
		ImVec2 windowPosition;
	} mainWindow;

	struct BombSettings {
		ImVec2 windowPosition;
	} bombWindow;
};

struct ConfigData {

	struct ActiveCheats {
		bool aimBot;
		bool antiFlash;
		bool bombHack;
		bool bunnyHop;
		bool esp;
		bool fovChanger;
		bool radar;
		bool triggerBot;
	} activeCheats;

	struct EspSettings {
		char selectedColor[COLOR_NAME_MAX_LENGTH];
		bool renderFriendlies;
		bool boxGlow;
		bool bone;
		bool boxesp;
		bool nameesp;

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
		bool ignoreteam;
		bool visible;
		ImVec4 fovCircleColor;
	} aimBot;

	struct FovSettings {
		int desiredFov;
	} fov;
};


extern ConfigData config;

extern PersistentConfigData persistentConfig;


bool load_cheat_config_data(ConfigData* data, const wchar_t* configName);
bool load_persistent_config_data(PersistentConfigData* data, const wchar_t* configName);

bool store_cheat_config_data(const ConfigData data, const wchar_t* configName);
bool store_persistent_config_data(const PersistentConfigData data, const wchar_t* configName);
