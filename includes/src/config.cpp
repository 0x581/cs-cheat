#include "../config.h"

#include <Windows.h>
#include <shlobj_core.h>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>

using std::filesystem::path;
using std::ifstream;
using std::ofstream;


static constexpr const ConfigData default_config{
	.activeCheats {
		.aimBot = true,
		.antiFlash = true,
		.bombHack = true,
		.bunnyHop = false,
		.esp = true,
		.fovChanger = false,
		.radar = true,
		.triggerBot = true,
	},
	.esp {
		.renderFriendlies = false,
		.boxGlow = false,
		.bone = true,
		.name = true,
		.health = true,
		.colorEnemyVisible = {1.0f, 0.0f, 0.0f, 1.0f},
		.colorEnemyHidden = {1.0f, 0.5f, 0.0f, 1.0f},
		.colorTeamVisible = {0.0f, 1.0f, 0.0f, 1.0f},
		.colorTeamHidden = {0.0f, 0.5f, 1.0f, 1.0f},
		.colorItem = {1.0f, 1.0f, 0.0f, 1.0f}
	},
	.aimBot {
		.pixelRadius = 100.f,
		.speedFactor = 0.7f,
		.drawFovCircle = true,
		.fovCircleColor = {1.0f, 0.0f, 1.0f, 0.7f}
	},
	.fov {.desiredFov = 90 },
};

static constexpr const PersistentConfigData default_shared_config{
	.mainWindow {.windowPosition = ImVec2(50, 100) },
	.bombWindow {.windowPosition = ImVec2(50, 50)},
};



ConfigData config = default_config;
PersistentConfigData persistentConfig = default_shared_config;

static bool createPathToConfig(path& cfg_path) {
	WCHAR* appdata{ 0 };

	if (!SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, 0, &appdata)))
	{
		CoTaskMemFree(appdata);
		std::cerr << "Error: LOCALAPPDATA environment variable not found.\n";
		return false;
	}

	cfg_path = path(appdata) / path(configFolder);
	CoTaskMemFree(appdata);

	// Ensure the directory exists
	std::filesystem::create_directories(cfg_path);

	return true;
}

template <typename T>
bool loadConfigData(T* buffer, const wchar_t* configName) {
	path cfg_path;

	if (!createPathToConfig(cfg_path))
		return false;

	const auto fileName = std::wstring(configName) + L".cfg";

	cfg_path /= fileName;

	std::ifstream file{ cfg_path, std::ifstream::ate | std::ios::binary };
	const auto fileSize = file.tellg();

	if (fileSize == -1)
	{
		std::cout << "No config found.\n";
		return false;
	}

	if (fileSize == 0)
	{
		std::cout << "Config is empty.\n";
		return false;
	}

	if (fileSize != sizeof(MAGIC_BYTES) + sizeof(T))
	{
		std::cout << "Error: Config file is either outdated or invalid.\n";
		return false;
	}

	file.seekg(0);

	char mBytesBuffer[sizeof(MAGIC_BYTES)]{ 0 };

	file.read(static_cast<char*>(mBytesBuffer), sizeof(MAGIC_BYTES));

	if (memcmp(MAGIC_BYTES, mBytesBuffer, sizeof(MAGIC_BYTES)))
	{
		std::cerr << "Error: Config file has invalid Magic Bytes.\n";
		return false;
	}

	memset(buffer, 0, sizeof(T));

	file.read(reinterpret_cast<char*>(buffer), sizeof(T));

	file.close();

	return true;
}

template <typename T>
bool storeConfigData(const T data, const wchar_t* configName) {
	path cfg_path;

	if (!createPathToConfig(cfg_path))
		return false;

	const auto fileName = std::wstring(configName) + L".cfg";

	cfg_path /= fileName;

	std::ofstream file{ cfg_path, std::ios::binary };

	file.write(MAGIC_BYTES, sizeof(MAGIC_BYTES));
	file.write(reinterpret_cast<const char*>(&data), sizeof(T));

	file.close();

	return true;
}

bool load_cheat_config_data(ConfigData* data, const wchar_t* configName) {
	return loadConfigData(data, configName);
}

bool load_persistent_config_data(PersistentConfigData* data, const wchar_t* configName) {
	return loadConfigData(data, configName);
}

bool store_cheat_config_data(const ConfigData data, const wchar_t* configName) {
	return storeConfigData(data, configName);
}

bool store_persistent_config_data(const PersistentConfigData data, const wchar_t* configName) {
	return storeConfigData(data, configName);
}