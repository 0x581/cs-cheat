#include "config_manager.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
using fs::path;
using std::ofstream;

template <typename T_PersistentConfig, typename T_Config>
bool ConfigManager<T_PersistentConfig, T_Config>::delete_config(const std::string_view config_name)
{
    if (config_name == files::persistentConfig)
    {
        if (verbose)
            std::cout << "Error: Cannot delete persistent config." << std::endl;
        return false;
    }

    if (!is_valid_config_name(config_name))
    {
        if (verbose)
            std::cout << "Error: Invalid config name: " << config_name << std::endl;
        return false;
    }

    if (config_name == currentConfig)
        reset_config();

    path cfg_path = workingDirectory / (std::string(config_name) + std::string(files::configExtension));

    if (!fs::exists(cfg_path))
        return false;

    const bool success = fs::remove(cfg_path);

    if (success)
        if (verbose)
            std::cout << "Deleted config: " << config_name << std::endl;
        else if (verbose)
            std::cout << "Failed to delete config: " << config_name << std::endl;

    update_available_configs();

    return success;
}
