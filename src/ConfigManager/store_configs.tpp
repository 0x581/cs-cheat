#include "config_manager.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "json.h"

#include "files.hpp"

namespace fs = std::filesystem;
using fs::path;
using std::ofstream;

template <typename T_PersistentConfig, typename T_Config>
void ConfigManager<T_PersistentConfig, T_Config>::store_persistent_config()
{
    const path cfg_path = workingDirectory / (std::string(files::persistentConfig) + std::string(files::configExtension));

    ofstream o_file{cfg_path, std::ios::binary};
    if (!o_file.is_open())
    {
        if (verbose)
            std::cout << "Error: Failed to store persistent config: " << cfg_path << std::endl;
        return;
    }

    auto j = nlohmann::json{
        {"Config type", PERSISTENT_CONFIG_KEYWORD},
        {"Selected Config", currentConfig}};

    j.update(persistentConfig);

    o_file << j.dump(4);

    o_file.flush();
    o_file.close();

    if (verbose)
        std::cout << "Stored persistent config." << std::endl;
}

template <typename T_PersistentConfig, typename T_Config>
void ConfigManager<T_PersistentConfig, T_Config>::store_cheat_config()
{
    if (currentConfig.empty())
    {
        if (verbose)
            std::cout << "Error: No current config set, cannot store cheat config." << std::endl;
        return;
    }

    if (currentConfig == files::persistentConfig)
    {
        if (verbose)
            std::cout << "Error: Current config is set to persistent, cannot store cheat config." << std::endl;
        return;
    }

    if (currentConfig == files::defaultConfig)
    {
        if (verbose)
            std::cout << "Error: Current config is set to default, cannot store cheat config." << std::endl;
        return;
    }

    const path cfg_path = workingDirectory / (std::string(currentConfig) + std::string(files::configExtension));

    ofstream o_file{cfg_path, std::ios::binary};
    if (!o_file.is_open())
    {
        if (verbose)
            std::cout << "Error: Failed to store cheat config: " << cfg_path << std::endl;
        return;
    }

    auto j = nlohmann::json{
        {"Config type", CHEAT_CONFIG_KEYWORD}};

    j.update(config);

    o_file << j.dump(4);

    o_file.flush();
    o_file.close();

    if (verbose)
        std::cout << "Stored cheat config: " << currentConfig << std::endl;

    update_available_configs();
}

template <typename T_PersistentConfig, typename T_Config>
void ConfigManager<T_PersistentConfig, T_Config>::store_all_config_data()
{
    store_persistent_config();

    store_cheat_config();
}
