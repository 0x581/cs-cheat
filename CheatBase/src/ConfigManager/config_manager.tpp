#include "config_manager.hpp"

#include "files.hpp"
#include "path_utilities.hpp"

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;
using fs::path;

template <typename T_PersistentConfig, typename T_Config>
ConfigManager<T_PersistentConfig, T_Config>::ConfigManager()
    : config{}, persistentConfig{}, workingDirectory{}, configs{}, currentConfig{files::defaultConfig}, verbose{false}
{
}

template <typename T_PersistentConfig, typename T_Config>
ConfigManager<T_PersistentConfig, T_Config>::ConfigManager(path folder) : ConfigManager()
{
    set_working_directory(folder);
}

template <typename T_PersistentConfig, typename T_Config>
bool ConfigManager<T_PersistentConfig, T_Config>::set_working_directory(const path &folder)
{
    path expanded;

    if (!get_expanded_path(folder.wstring(), expanded))
    {
        std::cerr << "Failed to expand working directory path: " << folder << "\n";
        return false;
    }

    if (!fs::exists(expanded) && !fs::create_directories(expanded))
    {
        std::cerr << "Failed to create working directory: " << expanded << "\n";
        return false;
    }

    workingDirectory = expanded;

    update_available_configs();

    return true;
}

template <typename T_PersistentConfig, typename T_Config>
bool ConfigManager<T_PersistentConfig, T_Config>::set_config_name(const std::string_view config_name)
{
    if (!is_valid_config_name(config_name))
    {
        if (verbose)
            std::cerr << "Invalid config name: " << config_name << "\n";
        return false;
    }

    currentConfig = std::string(config_name);
    return true;
}

template <typename T_PersistentConfig, typename T_Config>
void ConfigManager<T_PersistentConfig, T_Config>::reset_config()
{
    config = T_Config();
    set_config_name(files::defaultConfig);
}

template <typename T_PersistentConfig, typename T_Config>
void ConfigManager<T_PersistentConfig, T_Config>::update_available_configs()
{
    configs.clear();

    configs.push_back(std::string(files::defaultConfig));

    for (const auto &file : fs::directory_iterator(workingDirectory))
    {
        if (!file.is_regular_file())
            continue;

        const path &f_path = file.path();

        if (f_path.extension() != files::configExtension)
            continue;

        std::string name = f_path.stem().string();

        if (name == files::persistentConfig)
            continue;

        if (!is_valid_config_name(name))
            continue;

        configs.push_back(name);
    }
}

template <typename T_PersistentConfig, typename T_Config>
bool ConfigManager<T_PersistentConfig, T_Config>::is_valid_config_name(const std::string_view config_name) const
{
    if (config_name.empty() || config_name.length() > MAX_CONFIG_NAME_LENGTH)
        return false;

    for (const char c : config_name)
    {
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || (c == '_') || (c == '-') || (c == ' ')))
            return false;
    }

    return true;
}
