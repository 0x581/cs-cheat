#pragma once

#include "imgui.h"

#include <string_view>
#include <json.h>

constexpr auto CONFIG_NAME_MAX_LENGTH = 64;

struct ToggleableKeybind
{
	int key;
	bool isToggle = false;
	bool toggleState = false;
	bool keybindChanged = false;
};

struct PersistentConfigData
{
	static constexpr auto MAGIC_BYTES = std::string_view("Merkur CS2 Persistent Config\n");

	struct MainWindowSettings
	{
		ImVec2 position = ImVec2(50, 100);
		ImVec2 size = ImVec2(650, 500);
		float scale = 1.f;
	} mainWindow;

	struct BombSettings
	{
		ImVec2 position = ImVec2(50, 50);
		float scale = 1.f;
	} bombWindow;

	int targetFPS = 0;
	bool showFPS = false;

	char selectedConfig[CONFIG_NAME_MAX_LENGTH + 1] = { 0 };

	float esp_font_scale = 1.f;
};

struct ConfigData
{
	static constexpr auto MAGIC_BYTES = std::string_view("Merkur CS2 Cheat Config\n");

	struct ActiveCheats
	{
		bool aimBot = false;
		bool antiFlash = false;
		bool bombTimer = false;
		bool bunnyHop = false;
		bool esp = false;
		bool fovChanger = false;
		bool radar = false;
		bool triggerBot = false;
		bool rageBot = false;
	} activeCheats;

	struct EspSettings
	{
		bool renderFriendlies = false;
		bool boxGlow = false;
		bool bone = false;
		bool box = false;
		bool name = false;
		float font_scale = 1.f;
		bool health = false;
		float boneWidth = 1.f;

		ImVec4 colorEnemyVisible = { 1.0f, 0.0f, 0.0f, 1.0f };
		ImVec4 colorEnemyHidden = { 1.0f, 0.5f, 0.0f, 1.0f };
		ImVec4 colorTeamVisible = { 0.0f, 1.0f, 0.0f, 1.0f };
		ImVec4 colorTeamHidden = { 0.0f, 0.5f, 1.0f, 1.0f };
		ImVec4 colorItem = { 1.0f, 1.0f, 0.0f, 1.0f };
	} esp;

	struct AimBotSettings
	{
		float pixelRadius = 100.f;
		float speedFactor = 1.f;
		bool drawFovCircle = true;
		bool ignoreTeam = true;
		bool checkVisible = false;
		ImVec4 fovCircleColor = { 1.0f, 0.0f, 1.0f, 0.7f };
		int hitbox = 0;
		int shotDelay = 30;
	} aimBot;

	struct FovSettings
	{
		int desiredFov = 90;
	} fov;

	struct RcsSettings
	{
		bool  Rcs;
		float OldPunchAngleX = 0.0f;
		float OldPunchAngleY = 0.0f;
	} rcs;

	struct RageBotSettings
	{
		float pixelRadius = 100;
		float speedFactor = 1.f;
		bool checkVisible = false;
		int hitbox = 0;
		int shotDelay = 30;
	} rageBot;

	struct DevSettings
	{
		bool devMode = false;
	} devSettings;

	struct KeyBinds
	{
		int showMenu = VK_PRIOR;
		int quickExit = VK_NEXT;

		ToggleableKeybind bunnyHop = { VK_SPACE };
		ToggleableKeybind triggerBot = { VK_RBUTTON };
		ToggleableKeybind aimBot = { VK_RBUTTON };
		ToggleableKeybind rageBot = { VK_RBUTTON };
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
