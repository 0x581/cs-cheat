#pragma once

#include "imgui.h"

#include <string_view>
#include <json.h>

constexpr auto CONFIG_NAME_MAX_LENGTH = 64;

struct PersistentConfigData
{
	static constexpr auto MAGIC_BYTES = std::string_view("Merkur CS2 Persistent Config\n");

	struct MainWindowSettings
	{
		ImVec2 position;
		float scale;
	} mainWindow;

	struct BombSettings
	{
		ImVec2 position;
		float scale;
	} bombWindow;

	bool showFPS;

	char selectedConfig[CONFIG_NAME_MAX_LENGTH + 1];
};

struct ConfigData
{
	static constexpr auto MAGIC_BYTES = std::string_view("Merkur CS2 Cheat Config\n");

	struct ActiveCheats
	{
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

	struct EspSettings
	{
		bool renderFriendlies;
		bool boxGlow;
		bool bone;
		bool box;
		bool name;
		bool health;
		float boneWidth;

		ImVec4 colorEnemyVisible;
		ImVec4 colorEnemyHidden;
		ImVec4 colorTeamVisible;
		ImVec4 colorTeamHidden;
		ImVec4 colorItem;
	} esp;

	struct AimBotSettings
	{
		float pixelRadius;
		float speedFactor;
		bool drawFovCircle;
		bool ignoreTeam;
		bool checkVisible;
		ImVec4 fovCircleColor;
		int hitbox;
		int shotDelay = 30;
	} aimBot;

	struct FovSettings
	{
		int desiredFov;
	} fov;

	struct RageBotSettings
	{
		float pixelRadius;
		float speedFactor;
		bool visible;
		int hitbox;
		int shotDelay;
	} rageBot;

	struct DevSettings
	{
		bool devMode;
	} devSettings;

	struct KeyBinds
	{
		int showMenu;
		int quickExit;

		int bunnyHop;
		int triggerBot;
		int aimBot;
		int rageBot;
	} keybinds;
};


extern PersistentConfigData persistentConfig;
extern ConfigData config;

extern std::vector<std::string> configs;

void set_selected_config(const std::string_view config_name);

bool load_cheat_config_data(const std::string_view folder, const std::string_view config_name);

void store_all_config_data(const std::string_view folder);
void load_all_config_data(const std::string_view folder);

void store_persistent_config(const std::string_view folder);

void delete_config(const std::string_view folder, const std::string_view config_name);
