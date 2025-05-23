#include "../config.h"

#include <Windows.h>

#include <iostream>
#include <fstream>
#include <filesystem>

#include <files.h>
#include <strings.h>

#include <json.h>


namespace fs = std::filesystem;
using fs::path;
using std::ifstream;
using std::ofstream;

using std::string_view;


static constexpr const ConfigData default_config{
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
		.boneWidth = 1.0,
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
	.fov {
		.desiredFov = 90
	},
	.rageBot{
		.pixelRadius = 120.f,
		.speedFactor = 1.0f,
		.visible = false,
		.hitbox = 0,
		.shotDelay = 30,
	}
};

static constexpr const PersistentConfigData default_persistent_config{
	.mainWindow {
		.position = ImVec2(50, 100),
		.scale = 1.0,
	},
	.bombWindow {
		.position = ImVec2(50, 50),
		.scale = 1.0,
	},
	.selectedConfig{0},
};


ConfigData config = default_config;
PersistentConfigData persistentConfig = default_persistent_config;

std::vector<std::string> configs{};


static ImVec4 json_to_imvec4(const json& j, const ImVec4& default_value)
{
	if (j.is_array() && j.size() == 4)
	{
		return ImVec4(
			j[0].get<float>(),
			j[1].get<float>(),
			j[2].get<float>(),
			j[3].get<float>()
		);
	}

	return default_value;
}

static json imvec4_to_json(const ImVec4& vec)
{
	return json::array({ vec.x, vec.y, vec.z, vec.w });
}


static ImVec2 json_to_imvec2(const json& j, const ImVec2& default_value)
{
	if (j.is_array() && j.size() == 2)
	{
		return ImVec2(
			j[0].get<float>(),
			j[1].get<float>()
		);
	}
	return default_value;
}

static json imvec2_to_json(const ImVec2& vec)
{
	return json::array({ vec.x, vec.y });
}


static void from_json(PersistentConfigData& buffer, const json j, const bool verbose)
{
	try
	{
		auto& mainWindow = buffer.mainWindow;
		auto& bombWindow = buffer.bombWindow;
		auto& selectedConfig = buffer.selectedConfig;


		if (j.contains("mainWindow"))
		{
			const auto& mw = j["mainWindow"];

			mainWindow.position = json_to_imvec2(mw.value("position", json()), mainWindow.position);
			mainWindow.scale = mw.value("scale", default_persistent_config.mainWindow.scale);
		}
		else
			mainWindow = default_persistent_config.mainWindow;

		if (j.contains("bombWindow"))
		{
			const auto& bw = j["bombWindow"];

			bombWindow.position = json_to_imvec2(bw.value("position", json()), bombWindow.position);
			bombWindow.scale = bw.value("scale", default_persistent_config.bombWindow.scale);
		}
		else
			bombWindow = default_persistent_config.bombWindow;

		const auto selectedConfig_str = j.value("selectedConfig", std::string());

		const auto length = min(selectedConfig_str.length(), CONFIG_NAME_MAX_LENGTH);

		memset(selectedConfig, 0, CONFIG_NAME_MAX_LENGTH);
		memcpy(selectedConfig, selectedConfig_str.data(), length);
	}
	catch (const json::exception& e)
	{
		if (verbose)
			std::cerr << "Error: Failed to parse persistent config JSON: " << e.what() << std::endl;

		buffer = default_persistent_config;
	}
}

static json to_json(const PersistentConfigData& buffer)
{
	auto& mainWindow = buffer.mainWindow;
	auto& bombWindow = buffer.bombWindow;
	auto& selectedConfig = buffer.selectedConfig;

	json j;

	j["mainWindow"] = {
		{"position", imvec2_to_json(mainWindow.position)},
		{"scale", mainWindow.scale}
	};

	j["bombWindow"] = {
		{"position", imvec2_to_json(bombWindow.position)},
		{"scale", bombWindow.scale}
	};

	j["selectedConfig"] = std::string(selectedConfig);

	return j;
}


static void from_json(ConfigData& buffer, const json j, const bool verbose)
{
	try
	{
		auto& activeCheats = buffer.activeCheats;
		auto& esp = buffer.esp;
		auto& aimBot = buffer.aimBot;
		auto& fov = buffer.fov;
		auto& rageBot = buffer.rageBot;

		// Active Cheats
		if (j.contains("activeCheats"))
		{
			const auto& ac = j["activeCheats"];

			activeCheats.aimBot = ac.value("aimBot", default_config.activeCheats.aimBot);
			activeCheats.antiFlash = ac.value("antiFlash", default_config.activeCheats.antiFlash);
			activeCheats.bombTimer = ac.value("bombTimer", default_config.activeCheats.bombTimer);
			activeCheats.bunnyHop = ac.value("bunnyHop", default_config.activeCheats.bunnyHop);
			activeCheats.esp = ac.value("esp", default_config.activeCheats.esp);
			activeCheats.fovChanger = ac.value("fovChanger", default_config.activeCheats.fovChanger);
			activeCheats.radar = ac.value("radar", default_config.activeCheats.radar);
			activeCheats.triggerBot = ac.value("triggerBot", default_config.activeCheats.triggerBot);
			activeCheats.rageBot = ac.value("rageBot", default_config.activeCheats.rageBot);
		}
		else
			activeCheats = default_config.activeCheats;

		// ESP Settings
		if (j.contains("esp"))
		{
			const auto& espJson = j["esp"];

			esp.renderFriendlies = espJson.value("renderFriendlies", default_config.esp.renderFriendlies);
			esp.boxGlow = espJson.value("boxGlow", default_config.esp.boxGlow);
			esp.bone = espJson.value("bone", default_config.esp.bone);
			esp.box = espJson.value("box", default_config.esp.box);
			esp.name = espJson.value("name", default_config.esp.name);
			esp.health = espJson.value("health", default_config.esp.health);
			esp.boneWidth = espJson.value("boneWidth", default_config.esp.boneWidth);

			esp.colorEnemyVisible = json_to_imvec4(espJson.value("colorEnemyVisible", json()), esp.colorEnemyVisible);
			esp.colorEnemyHidden = json_to_imvec4(espJson.value("colorEnemyHidden", json()), esp.colorEnemyHidden);
			esp.colorTeamVisible = json_to_imvec4(espJson.value("colorTeamVisible", json()), esp.colorTeamVisible);
			esp.colorTeamHidden = json_to_imvec4(espJson.value("colorTeamHidden", json()), esp.colorTeamHidden);
			esp.colorItem = json_to_imvec4(espJson.value("colorItem", json()), esp.colorItem);
		}
		else
			esp = default_config.esp;

		// AimBot Settings
		if (j.contains("aimBot"))
		{
			const auto& aimJson = j["aimBot"];

			aimBot.pixelRadius = aimJson.value("pixelRadius", default_config.aimBot.pixelRadius);
			aimBot.speedFactor = aimJson.value("speedFactor", default_config.aimBot.speedFactor);
			aimBot.drawFovCircle = aimJson.value("drawFovCircle", default_config.aimBot.drawFovCircle);
			aimBot.ignoreTeam = aimJson.value("ignoreTeam", default_config.aimBot.ignoreTeam);
			aimBot.checkVisible = aimJson.value("checkVisible", default_config.aimBot.checkVisible);
			aimBot.hitbox = aimJson.value("hitbox", default_config.aimBot.hitbox);
			aimBot.shotDelay = aimJson.value("shotDelay", default_config.aimBot.shotDelay);

			aimBot.fovCircleColor = json_to_imvec4(aimJson.value("fovCircleColor", json()), aimBot.fovCircleColor);
		}
		else
			aimBot = default_config.aimBot;

		// FOV Settings
		if (j.contains("fov"))
		{
			const auto& fovJson = j["fov"];

			fov.desiredFov = fovJson.value("desiredFov", default_config.fov.desiredFov);
		}
		else
			fov = default_config.fov;

		// RageBot Settings
		if (j.contains("rageBot"))
		{
			const auto& rageJson = j["rageBot"];

			rageBot.pixelRadius = rageJson.value("pixelRadius", default_config.rageBot.pixelRadius);
			rageBot.speedFactor = rageJson.value("speedFactor", default_config.rageBot.speedFactor);
			rageBot.visible = rageJson.value("visible", default_config.rageBot.visible);
			rageBot.hitbox = rageJson.value("hitbox", default_config.rageBot.hitbox);
			rageBot.shotDelay = rageJson.value("shotDelay", default_config.rageBot.shotDelay);
		}
		else
			rageBot = default_config.rageBot;

	}
	catch (const json::exception& e)
	{
		if (verbose)
			std::cerr << "Error: Failed to parse config JSON: " << e.what() << std::endl;

		buffer = default_config;
	}
}

static json to_json(const ConfigData& buffer)
{
	auto& activeCheats = buffer.activeCheats;
	auto& esp = buffer.esp;
	auto& aimBot = buffer.aimBot;
	auto& fov = buffer.fov;
	auto& rageBot = buffer.rageBot;

	json j;

	// Active Cheats
	j["activeCheats"] = {
		{"aimBot", activeCheats.aimBot},
		{"antiFlash", activeCheats.antiFlash},
		{"bombTimer", activeCheats.bombTimer},
		{"bunnyHop", activeCheats.bunnyHop},
		{"esp", activeCheats.esp},
		{"fovChanger", activeCheats.fovChanger},
		{"radar", activeCheats.radar},
		{"triggerBot", activeCheats.triggerBot},
		{"rageBot", activeCheats.rageBot}
	};

	// ESP Settings
	j["esp"] = {
		{"renderFriendlies", esp.renderFriendlies},
		{"boxGlow", esp.boxGlow},
		{"bone", esp.bone},
		{"box", esp.box},
		{"name", esp.name},
		{"health", esp.health},
		{"boneWidth", esp.boneWidth},
		{"colorEnemyVisible", imvec4_to_json(esp.colorEnemyVisible)},
		{"colorEnemyHidden", imvec4_to_json(esp.colorEnemyHidden)},
		{"colorTeamVisible", imvec4_to_json(esp.colorTeamVisible)},
		{"colorTeamHidden", imvec4_to_json(esp.colorTeamHidden)},
		{"colorItem", imvec4_to_json(esp.colorItem)}
	};

	// AimBot Settings
	j["aimBot"] = {
		{"pixelRadius", aimBot.pixelRadius},
		{"speedFactor", aimBot.speedFactor},
		{"drawFovCircle", aimBot.drawFovCircle},
		{"ignoreTeam", aimBot.ignoreTeam},
		{"checkVisible", aimBot.checkVisible},
		{"fovCircleColor", imvec4_to_json(aimBot.fovCircleColor)},
		{"hitbox", aimBot.hitbox},
		{"shotDelay", aimBot.shotDelay}
	};

	// FOV Settings
	j["fov"] = {
		{"desiredFov", fov.desiredFov}
	};

	// RageBot Settings
	j["rageBot"] = {
		{"pixelRadius", rageBot.pixelRadius},
		{"speedFactor", rageBot.speedFactor},
		{"visible", rageBot.visible},
		{"hitbox", rageBot.hitbox},
		{"shotDelay", rageBot.shotDelay}
	};

	return j;
}



static bool get_expanded_path(const string_view folder, path& expandedPath)
{
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

static bool get_path_and_ensure_dir_exists(const string_view folder, const string_view file, path& expandedPath)
{
	if (!get_expanded_path(folder, expandedPath))
		return false;

	if (!fs::exists(expandedPath))
		fs::create_directories(expandedPath);

	std::string file_name{ file };
	file_name += files::configExtension;

	expandedPath /= file_name;

	return true;
}


template <typename T>
static bool is_valid_config(const path& path, const bool verbose = false)
{
	if (!fs::exists(path))
	{
		if (verbose)
			std::cerr << "Error: File does not exist: " << path << std::endl;
		return false;
	}

	if (path.extension() != files::configExtension)
	{
		if (verbose)
			std::cerr << "Error: Invalid extension: " << path << std::endl;
		return false;
	}

	ifstream i_file{ path, std::ifstream::ate | std::ios::binary };
	if (!i_file.is_open())
	{
		if (verbose)
			std::cerr << "Error: Failed to open config file: " << path << std::endl;
		return false;
	}

	char buffer[T::MAGIC_BYTES.size()];
	i_file.seekg(0);
	i_file.read(buffer, T::MAGIC_BYTES.size());
	i_file.close();

	if (memcmp(T::MAGIC_BYTES.data(), buffer, T::MAGIC_BYTES.size()) != 0)
	{
		if (verbose)
			std::cerr << "Error: Config file does not contain the correct magic bytes: " << path << std::endl;
		return false;
	}

	return true;
}

template <typename T>
static bool load_config_data(T& buffer, const string_view folder, const string_view config_name, const bool verbose = false)
{
	path cfg_path;
	if (!get_path_and_ensure_dir_exists(folder, config_name, cfg_path))
	{
		if (verbose)
			std::cerr << "Error: Failed to get folder path.\n";
		return false;
	}

	if (!is_valid_config<T>(cfg_path, verbose))
		return false;

	ifstream i_file{ cfg_path, std::ifstream::ate | std::ios::binary };
	if (!i_file.is_open())
	{
		if (verbose)
			std::cerr << "Error: Failed to open config file: " << cfg_path << std::endl;
		return false;
	}

	i_file.seekg(T::MAGIC_BYTES.size());

	json j;
	try
	{
		i_file >> j;
	}
	catch (const json::exception& e)
	{
		if (verbose)
			std::cerr << "Error: Failed to parse config JSON: " << e.what() << std::endl;

		i_file.close();
		return false;
	}

	i_file.close();

	from_json(buffer, j, verbose);

	return true;
}

template <typename T>
static bool store_config_data(const T& data, const string_view folder, const string_view config_name, const bool verbose = false)
{
	if (config_name == files::defaultConfig)
	{
		if (verbose)
			std::cout << "Error: Cannot store default config." << std::endl;
		return false;
	}

	path cfg_path;
	if (!get_path_and_ensure_dir_exists(folder, config_name, cfg_path))
	{
		if (verbose)
			std::cout << "Error: Failed to get folder path.\n";
		return false;
	}

	ofstream o_file{ cfg_path, std::ios::binary };
	if (!o_file.is_open())
	{
		if (verbose)
			std::cout << "Error: Failed to open output config file: " << cfg_path << std::endl;
		return false;
	}

	o_file << T::MAGIC_BYTES << to_json(data).dump(4);

	o_file.flush();
	o_file.close();

	return true;
}


static void update_available_configs(const string_view folder)
{
	configs.clear();

	path cfg_path;
	if (!get_expanded_path(folder, cfg_path))
	{
		std::cerr << "Error: Failed to expand folder path.\n";
		return;
	}

	if (!fs::exists(cfg_path))
		fs::create_directories(cfg_path);

	for (const auto& file : fs::directory_iterator(cfg_path))
	{
		if (!file.is_regular_file())
			continue;

		const path& f_path = file.path();

		if (!is_valid_config<ConfigData>(f_path))
			continue;

		configs.push_back(f_path.stem().string());
	}
}

void set_selected_config(const string_view config_name)
{
	if (config_name.length() > CONFIG_NAME_MAX_LENGTH)
	{
		std::cout << "Error: Config name too long: " << config_name << std::endl;
		return;
	}

	const auto length = config_name.length();
	const auto remaining_length = CONFIG_NAME_MAX_LENGTH - length;

	memcpy(persistentConfig.selectedConfig, config_name.data(), length);
	memset(persistentConfig.selectedConfig + length, 0, remaining_length);
}


static void load_default_config_data()
{
	memcpy(&config, &default_config, sizeof(ConfigData));
	set_selected_config(files::defaultConfig);

	std::cout << "Default config loaded from memory." << std::endl;
}

bool load_cheat_config_data(const string_view folder, const string_view config_name)
{
	if (config_name.length() > CONFIG_NAME_MAX_LENGTH)
	{
		std::cout << "Error: Config name too long: " << config_name << std::endl;
		return false;
	}

	if (config_name == files::defaultConfig)
	{
		load_default_config_data();
		return true;
	}

	if (load_config_data(config, folder, config_name))
	{
		set_selected_config(config_name);
		std::cout << "Loaded config: " << config_name << std::endl;
		return true;
	}

	return false;
}

void load_all_config_data(const string_view folder)
{
	update_available_configs(folder);

	const auto persistent_exists = load_config_data(persistentConfig, folder, files::persistentConfig);

	if (persistent_exists)
		std::cout << "Loaded persistent config." << std::endl;
	else
		std::cout << "Failed to load persistent config." << std::endl;


	if (persistent_exists)
	{
		if (load_cheat_config_data(folder, persistentConfig.selectedConfig))
			return;
	}

	load_default_config_data();
}


void store_persistent_config(const string_view folder)
{
	if (store_config_data(persistentConfig, folder, files::persistentConfig))
		std::cout << "Stored persistent config." << std::endl;
	else
		std::cout << "Failed to store persistent config." << std::endl;
}

void store_all_config_data(const string_view folder)
{

	path cfg_path;
	store_persistent_config(folder);

	if (store_config_data(config, folder, persistentConfig.selectedConfig))
		std::cout << "Stored cheat config: " << persistentConfig.selectedConfig << std::endl;
	else
		std::cout << "Failed to store cheat config: " << persistentConfig.selectedConfig << std::endl;

	update_available_configs(folder);
}


void delete_config(const std::string_view folder, const std::string_view config_name)
{
	if (config_name == persistentConfig.selectedConfig)
		load_default_config_data();

	path cfg_path;
	if (!get_expanded_path(folder, cfg_path))
	{
		std::cout << "Error: Failed to expand folder path.\n";
		return;
	}

	if (!fs::exists(cfg_path))
		fs::create_directories(cfg_path);

	const auto file_name = std::string(config_name) + std::string(files::configExtension);
	cfg_path /= file_name;

	if (!is_valid_config<ConfigData>(cfg_path))
		std::cout << "Not a valid config file: " << cfg_path << std::endl;
	else if (fs::remove(cfg_path))
		std::cout << "Deleted config: " << config_name << std::endl;
	else
		std::cout << "Failed to delete config: " << config_name << std::endl;

	update_available_configs(folder);
}
