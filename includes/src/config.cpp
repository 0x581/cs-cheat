#include "../config.h"

#include <Windows.h>
#include <shlobj_core.h>

#include <iostream>
#include <fstream>
#include <filesystem>

using std::filesystem::path;
using std::ifstream;
using std::ofstream;


static constexpr const ConfigData default_config{
	._magicBytes {MAGIC_BYTES},
	.activeCheats {
		.aimBot = true,
		.antiFlash = true,
		.bombTimer = true,
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
	._magicBytes {MAGIC_BYTES},
	.mainWindow {.windowPosition = ImVec2(50, 100)},
	.bombWindow {.windowPosition = ImVec2(50, 50)},
};



ConfigData config = default_config;
PersistentConfigData persistentConfig = default_shared_config;


static bool get_expanded_path(const wchar_t* folder, path& expandedPath) {
	const auto required_size = ExpandEnvironmentStrings(folder, nullptr, 0);

	if (required_size == 0)
		return false;

	wchar_t* folder_expanded = new wchar_t[required_size];
	const auto parsed_length = ExpandEnvironmentStrings(folder, folder_expanded, required_size);

	if (parsed_length != required_size)
	{
		delete[] folder_expanded;
		return false;
	}

	expandedPath = std::filesystem::absolute(folder_expanded);
	delete[] folder_expanded;

	return true;
}


template <typename T>
static bool loadConfigData(T* buffer, const wchar_t* folder, const wchar_t* file) {
	path cfg_path;
	if (!get_expanded_path(folder, cfg_path))
	{
		std::cout << "Error: Failed to expand folder path.\n";
		return false;
	}

	if (!std::filesystem::exists(cfg_path))
		std::filesystem::create_directories(cfg_path);

	cfg_path /= file;

	if (!std::filesystem::exists(cfg_path))
	{
		std::cout << "Config file not found.\n";
		return false;
	}

	ifstream i_file{ cfg_path, std::ifstream::ate | std::ios::binary };

	if (i_file.tellg() != sizeof(T))
	{
		std::cout << "Error: Config file is either outdated or invalid.\n";
		i_file.close();
		return false;
	}

	i_file.seekg(0);

	memset(buffer, 0, sizeof(T));
	i_file.read(reinterpret_cast<char*>(buffer), sizeof(T));
	i_file.close();

	if (memcmp(MAGIC_BYTES, buffer->_magicBytes, sizeof(MAGIC_BYTES)))
	{
		std::cerr << "Error: Config file has invalid Magic Bytes.\n";
		return false;
	}

	return true;
}

template <typename T>
static bool storeConfigData(const T data, const wchar_t* folder, const wchar_t* file) {
	path cfg_path;
	if (!get_expanded_path(folder, cfg_path))
	{
		std::cout << "Error: Failed to expand folder path.\n";
		return false;
	}

	if (!std::filesystem::exists(cfg_path))
		std::filesystem::create_directories(cfg_path);

	cfg_path /= file;

	ofstream o_file{ cfg_path, std::ios::binary };

	o_file.write(reinterpret_cast<const char*>(&data), sizeof(T));

	o_file.close();

	return true;
}

bool load_cheat_config_data(ConfigData* data, const wchar_t* folder, const wchar_t* file) {
	return loadConfigData(data, folder, file);
}

bool load_persistent_config_data(PersistentConfigData* data, const wchar_t* folder, const wchar_t* file) {
	return loadConfigData(data, folder, file);
}

bool store_cheat_config_data(const ConfigData data, const wchar_t* folder, const wchar_t* file) {
	return storeConfigData(data, folder, file);
}

bool store_persistent_config_data(const PersistentConfigData data, const wchar_t* folder, const wchar_t* file) {
	return storeConfigData(data, folder, file);
}