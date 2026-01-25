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

ConfigData config = ConfigData();
PersistentConfigData persistentConfig = PersistentConfigData();

std::vector<std::string> configs{};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ImVec2, x, y)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ImVec4, x, y, z, w)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ToggleableKeybind, key, isToggle, toggleState)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PersistentConfigData::MainWindowSettings, position, size, scale)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PersistentConfigData::BombSettings, position, scale)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PersistentConfigData, mainWindow, bombWindow, targetFPS, showFPS, selectedConfig, esp_font_scale)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ConfigData::ActiveCheats, aimBot, antiFlash, bombTimer, bunnyHop, esp, fovChanger, radar, triggerBot, rageBot)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ConfigData::EspSettings, renderFriendlies, boxGlow, bone, box, name, font_scale, health, boneWidth, colorEnemyVisible, colorEnemyHidden, colorTeamVisible, colorTeamHidden, colorItem)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ConfigData::AimBotSettings, pixelRadius, speedFactor, drawFovCircle, ignoreTeam, checkVisible, fovCircleColor, hitbox, shotDelay)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ConfigData::FovSettings, desiredFov)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ConfigData::RcsSettings, Rcs, OldPunchAngleX, OldPunchAngleY)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ConfigData::RageBotSettings, pixelRadius, speedFactor, checkVisible, hitbox, shotDelay)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ConfigData::DevSettings, devMode)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ConfigData::KeyBinds, showMenu, quickExit, bunnyHop, triggerBot, aimBot, rageBot)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ConfigData, activeCheats, esp, aimBot, fov, rcs, rageBot, devSettings, keybinds)

static bool get_expanded_path(const string_view folder, path &expandedPath)
{
    const auto wstring_folder = string_to_wstring(folder);

    const auto required_size = ExpandEnvironmentStrings(wstring_folder.c_str(), nullptr, 0);

    if (required_size == 0)
        return false;

    wchar_t *folder_expanded = new wchar_t[required_size];
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

static bool get_path_and_ensure_dir_exists(const string_view folder, const string_view file, path &expandedPath)
{
    if (!get_expanded_path(folder, expandedPath))
        return false;

    if (!fs::exists(expandedPath))
        fs::create_directories(expandedPath);

    std::string file_name{file};
    file_name += files::configExtension;

    expandedPath /= file_name;

    return true;
}

template <typename T>
static bool is_valid_config(const path &path, const bool verbose = false)
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

    ifstream i_file{path, std::ifstream::ate | std::ios::binary};
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
static bool load_config_data(T &buffer, const string_view folder, const string_view config_name, const bool verbose = false)
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

    ifstream i_file{cfg_path, std::ifstream::ate | std::ios::binary};
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
    catch (const json::exception &e)
    {
        if (verbose)
            std::cerr << "Error: Failed to parse config JSON: " << e.what() << std::endl;

        i_file.close();
        return false;
    }

    i_file.close();

    try
    {
        buffer = j.get<T>();
    }
    catch (const json::exception &e)
    {
        if (verbose)
            std::cerr << "Error: Failed to convert JSON to config data: " << e.what() << std::endl;
        return false;
    }

    return true;
}

template <typename T>
static bool store_config_data(const T &data, const string_view folder, const string_view config_name, const bool verbose = false)
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

    ofstream o_file{cfg_path, std::ios::binary};
    if (!o_file.is_open())
    {
        if (verbose)
            std::cout << "Error: Failed to open output config file: " << cfg_path << std::endl;
        return false;
    }

    o_file << T::MAGIC_BYTES << nlohmann::json(data).dump(4);

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

    for (const auto &file : fs::directory_iterator(cfg_path))
    {
        if (!file.is_regular_file())
            continue;

        const path &f_path = file.path();

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

    persistentConfig.selectedConfig = config_name;
}

static void load_default_config_data()
{
    config = ConfigData();
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
