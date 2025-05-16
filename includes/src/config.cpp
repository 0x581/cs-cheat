#include "../config.h"

#include <Windows.h>

#include <iostream>
#include <fstream>
#include <filesystem>

#include <files.h>
#include <strings.h>


namespace fs = std::filesystem;
using fs::path;
using std::ifstream;
using std::ofstream;

using std::string_view;


static constexpr const ConfigData default_config{
	._magicBytes {MAGIC_BYTES},
	.activeCheats {
		.aimBot = false,
		.antiFlash = false,
		.bombTimer = false,
		.bunnyHop = false,
		.esp = false,
		.fovChanger = false,
		.radar = false,
		.triggerBot = false,
	},
	.esp {
		.renderFriendlies = false,
		.boxGlow = false,
		.bone = false,
		.name = false,
		.health = false,
		.colorEnemyVisible = {1.0f, 0.0f, 0.0f, 1.0f},
		.colorEnemyHidden = {1.0f, 0.5f, 0.0f, 1.0f},
		.colorTeamVisible = {0.0f, 1.0f, 0.0f, 1.0f},
		.colorTeamHidden = {0.0f, 0.5f, 1.0f, 1.0f},
		.colorItem = {1.0f, 1.0f, 0.0f, 1.0f}
	},
	.aimBot {
		.pixelRadius = 100.f,
		.speedFactor = 0.7f,
		.drawFovCircle = false,
		.fovCircleColor = {1.0f, 0.0f, 1.0f, 0.7f}
	},
	.fov {.desiredFov = 90 },
};

static constexpr const PersistentConfigData default_shared_config{
	._magicBytes {MAGIC_BYTES},
	.mainWindow {.windowPosition = ImVec2(50, 100)},
	.bombWindow {.windowPosition = ImVec2(50, 50)},
	.selectedConfig{0},
};



ConfigData config = default_config;
PersistentConfigData persistentConfig = default_shared_config;

std::vector<std::string> configs{};

template <typename T>
static bool is_valid_config(const path path, const bool verbose = false) {
	if (!fs::exists(path))
	{
		if (verbose)
			std::cout << "Error: File does not exist: " << path << std::endl;
		return false;
	}

	if (path.extension() != files::configExtension)
	{
		if (verbose)
			std::cout << "Error: Invalid extension: " << path << std::endl;
		return false;
	}

	ifstream i_file{ path, std::ifstream::ate | std::ios::binary };
	if (!i_file.is_open())
	{
		if (verbose)
			std::cout << "Error: Failed to open config file: " << path << std::endl;
		return false;
	}

	const auto file_size = i_file.tellg();
	if (file_size != sizeof(T))
	{
		if (verbose)
			std::cout << "Error: Config file size is invalid (" << file_size << " instead of " << sizeof(T) << "): " << path << std::endl;
		i_file.close();
		return false;
	}

	char buffer[sizeof(MAGIC_BYTES)];
	i_file.seekg(0);
	i_file.read(buffer, sizeof(MAGIC_BYTES));


	if (memcmp(MAGIC_BYTES, buffer, sizeof(MAGIC_BYTES)))
	{
		if (verbose)
			std::cout << "Error: Config file does not contain the correct magic bytes: " << path << std::endl;
		i_file.close();
		return false;
	}

	i_file.close();
	return true;
}

static bool get_expanded_path(const string_view folder, path& expandedPath) {
	const auto wstring_folder = string_to_wstring(folder);

	const auto required_size = ExpandEnvironmentStrings(wstring_folder.c_str(), nullptr, 0);

	if (required_size == 0)
		return false;

	wchar_t* folder_expanded = new wchar_t[required_size];
	const auto parsed_length = ExpandEnvironmentStrings(wstring_folder.c_str(), folder_expanded, required_size);

	if (parsed_length != required_size)
	{
		delete[] folder_expanded;
		return false;
	}

	expandedPath = fs::absolute(folder_expanded);
	delete[] folder_expanded;

	return true;
}


template <typename T>
static bool load_config_data(T& buffer, const string_view folder, const string_view file) {
	path cfg_path;
	if (!get_expanded_path(folder, cfg_path))
	{
		std::cout << "Error: Failed to expand folder path.\n";
		return false;
	}

	if (!fs::exists(cfg_path))
		fs::create_directories(cfg_path);

	const auto file_name = std::string(file) + std::string(files::configExtension);
	cfg_path /= file_name;

	if (!is_valid_config<T>(cfg_path, true))
		return false;

	ifstream i_file{ cfg_path, std::ifstream::ate | std::ios::binary };
	if (!i_file.is_open())
	{
		std::cout << "Error: Failed to open config file: " << cfg_path << std::endl;
		return false;
	}

	i_file.seekg(0);
	i_file.read((char*)(&buffer), sizeof(T));
	i_file.close();

	return true;
}

template <typename T>
static bool store_config_data(const T& data, const string_view folder, const string_view file) {
	path cfg_path;
	if (!get_expanded_path(folder, cfg_path))
	{
		std::cout << "Error: Failed to expand folder path.\n";
		return false;
	}

	if (!fs::exists(cfg_path))
		fs::create_directories(cfg_path);

	const auto file_name = std::string(file) + std::string(files::configExtension);
	cfg_path /= file_name;

	ofstream o_file{ cfg_path, std::ios::binary };
	if (!o_file.is_open())
	{
		std::cout << "Error: Failed to open output config file: " << cfg_path << std::endl;
		return false;
	}

	o_file.write((char*)(&data), sizeof(T));

	o_file.flush();

	o_file.close();

	return true;
}

static void update_available_configs(const string_view folder) {
	configs.clear();

	path cfg_path;
	if (!get_expanded_path(folder, cfg_path))
	{
		std::cout << "Error: Failed to expand folder path.\n";
		return;
	}

	if (!fs::exists(cfg_path))
		fs::create_directories(cfg_path);

	for (const auto file : fs::directory_iterator(cfg_path))
	{
		if (!file.is_regular_file())
			continue;

		const path f_path = file.path();

		if (!is_valid_config<ConfigData>(f_path))
			continue;

		configs.push_back(f_path.stem().string());
	}
}

void set_selected_config(const string_view config_name) {
	if (config_name.length() > CONFIG_NAME_MAX_LENGTH)
	{
		std::cout << "Error: Config name too long: " << config_name << std::endl;
		return;
	}

	memset(persistentConfig.selectedConfig, 0, sizeof(persistentConfig.selectedConfig));
	memcpy(persistentConfig.selectedConfig, config_name.data(), config_name.length());
}

bool load_cheat_config_data(const string_view folder, const string_view config_name) {
	if (config_name.length() > CONFIG_NAME_MAX_LENGTH)
	{
		std::cout << "Error: Config name too long: " << config_name << std::endl;
		return false;
	}

	if (load_config_data(config, folder, config_name))
	{
		set_selected_config(config_name);
		std::cout << "Loaded config: " << config_name << std::endl;
		return true;
	}

	return false;
}


void load_all_config_data(const string_view folder) {
	update_available_configs(folder);

	const auto persistent_exists = load_config_data(persistentConfig, folder, files::persistentConfig);

	if (persistent_exists)
		std::cout << "Loaded persistent config." << std::endl;
	else
		std::cout << "Failed to load persistent config." << std::endl;


	if (persistent_exists)
	{
		const std::string selected_config{ persistentConfig.selectedConfig };

		if (load_cheat_config_data(folder, selected_config))
			return;
	}

	set_selected_config(files::defaultConfig);
	load_cheat_config_data(folder, files::defaultConfig);
}

void store_all_config_data(const string_view folder) {
	if (store_config_data(persistentConfig, folder, files::persistentConfig))
		std::cout << "Stored persistent config." << std::endl;
	if (store_config_data(config, folder, string_view(persistentConfig.selectedConfig)))
		std::cout << "Stored cheat config: " << persistentConfig.selectedConfig << std::endl;

	update_available_configs(folder);
}
