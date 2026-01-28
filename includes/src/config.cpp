#include "config.hpp"

ConfigManagerInstance configManager{};

PersistentConfigData &persistentConfig = configManager.persistentConfig;
ConfigData &config = configManager.config;
