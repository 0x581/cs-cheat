#include "config_manager.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "json.h"

#include "files.hpp"

namespace fs = std::filesystem;
using fs::path;
using std::ifstream;

template <typename T_PersistentConfig, typename T_CheatConfig>
std::string ConfigManager<T_PersistentConfig, T_CheatConfig>::load_persistent_config()
{
    const path cfg_path = workingDirectory / (std::string(files::persistentConfig) + std::string(files::configExtension));

    ifstream i_file{cfg_path, std::ios::binary};
    if (!i_file.is_open())
    {
        if (verbose)
            std::cout << "Error: Failed to open persistent config: " << cfg_path << std::endl;
        return "";
    }

    try
    {
        json j;

        i_file >> j;

        if (j["Config type"] != std::string(PERSISTENT_CONFIG_KEYWORD))
        {
            if (verbose)
                std::cerr << "Error: Not a persistent config file." << std::endl;

            return "";
        }

        persistent = j.get<T_PersistentConfig>();

        if (verbose)
            std::cout << "Loaded persistent config." << std::endl;

        return j["Selected Config"].get<std::string>();
    }
    catch (const json::exception &e)
    {
        if (verbose)
            std::cerr << "Error: Failed to parse config JSON: " << e.what() << std::endl;

        return "";
    };
}

template <typename T_PersistentConfig, typename T_CheatConfig>
void ConfigManager<T_PersistentConfig, T_CheatConfig>::load_cheat_config(std::string_view config_name)
{
    if (!is_valid_config_name(config_name))
    {
        if (verbose)
            std::cerr << "Error: Invalid config name: " << config_name << std::endl;
        return;
    }

    if (config_name == files::defaultConfig)
    {
        reset_config();
        return;
    }

    const path cfg_path = workingDirectory / (std::string(config_name) + std::string(files::configExtension));

    ifstream i_file{cfg_path, std::ios::binary};
    if (!i_file.is_open())
    {
        if (verbose)
            std::cout << "Error: Failed to open cheat config: " << config_name << std::endl;
        return;
    }

    try
    {
        json j;

        i_file >> j;

        if (j["Config type"] != std::string(CHEAT_CONFIG_KEYWORD))
        {
            if (verbose)
                std::cerr << "Error: Not a cheat config file." << std::endl;

            return;
        }

        cheat = j.get<T_CheatConfig>();

        set_config_name(config_name);

        if (verbose)
            std::cout << "Loaded cheat config." << std::endl;
    }
    catch (const json::exception &e)
    {
        if (verbose)
            std::cerr << "Error: Failed to parse config JSON: " << e.what() << std::endl;
    }
}

template <typename T_PersistentConfig, typename T_CheatConfig>
void ConfigManager<T_PersistentConfig, T_CheatConfig>::load_all_config_data()
{
    const auto config = load_persistent_config();

    if (verbose)
        std::cout << "Persistent config loaded, loading cheat config: " << config << std::endl;

    if (config.empty() || config == files::defaultConfig)
        reset_config();
    else
        load_cheat_config(config);
}
