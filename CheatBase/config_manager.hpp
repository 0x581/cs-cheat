#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

template <typename T_PersistentConfig, typename T_Config>
class ConfigManager
{
private:
public:
    static constexpr int MAX_CONFIG_NAME_LENGTH = 64;
    static constexpr auto PERSISTENT_CONFIG_KEYWORD = std::string_view("Merkur Persistent Config");
    static constexpr auto CHEAT_CONFIG_KEYWORD = std::string_view("Merkur Cheat Config");

    T_PersistentConfig persistentConfig;
    T_Config config;

    std::filesystem::path workingDirectory;

    std::vector<std::string> configs;
    std::string currentConfig;

    bool verbose = false;

    ConfigManager();
    ConfigManager(std::filesystem::path folder);
    ConfigManager(std::string_view folder) : ConfigManager(std::filesystem::path(folder)) {}

    bool set_working_directory(const std::filesystem::path &folder);
    bool set_working_directory(const std::string_view folder) { return set_working_directory(std::filesystem::path(folder)); }

    bool set_config_name(const std::string_view config_name);

    std::string load_persistent_config();
    void load_cheat_config(const std::string_view config_name);

    void store_persistent_config();
    void store_cheat_config();

    void load_all_config_data();
    void store_all_config_data();

    bool delete_config(const std::string_view config_name);

    void reset_config();

    void update_available_configs();

private:
    bool is_valid_config_name(const std::string_view config_name) const;
};

#include "src/ConfigManager/config_manager.tpp"
#include "src/ConfigManager/load_configs.tpp"
#include "src/ConfigManager/store_configs.tpp"
#include "src/ConfigManager/delete_config.tpp"
