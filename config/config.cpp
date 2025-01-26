#include "config.hpp"

#include <Windows.h>
#include <shlobj_core.h>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>

using std::filesystem::path;
using std::ifstream;
using std::ofstream;

constexpr char MAGIC_BYTES[64]{ "Basic CS2 Cheat config" };
constexpr auto configPath = "Basic CS2 Cheat/config.cfg"; // In Local Appdata

static bool createPathIfNotExist(path &cfg_path, const char* pathToFile = configPath)
{
    WCHAR *appdata{ 0 };

    if (!SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, 0, &appdata)))
    {
        CoTaskMemFree(appdata);
        std::cerr << "Error: LOCALAPPDATA environment variable not found.\n";
        return false;
    }

    cfg_path = path(appdata) / path(pathToFile);
    CoTaskMemFree(appdata);

    // Ensure the directory exists
    std::filesystem::create_directories(cfg_path.parent_path());

    return true;
}

bool loadConfigData(ConfigData* buffer) {
    path cfg_path;

    if (!createPathIfNotExist(cfg_path))
        return false;

    ifstream file{ cfg_path, std::ifstream::ate | std::ios::binary };
    const auto fileSize = file.tellg();

    if (fileSize == 0)
    {
        std::cout << "No config found.\n";
        return false;
    }

    if (fileSize != sizeof(MAGIC_BYTES) + sizeof(ConfigData))
    {
        std::cerr << "Error: Config file is either outdated or invalid.\n";
        return false;
    }

    file.seekg(0);

    char mBytesBuffer[sizeof(MAGIC_BYTES)]{ 0 };

    file.read(static_cast<char *>(mBytesBuffer), sizeof(MAGIC_BYTES));

    if (memcmp(MAGIC_BYTES, mBytesBuffer, sizeof(MAGIC_BYTES)))
    {
        std::cerr << "Error: Config file has invalid Magic Bytes.\n";
        return false;
    }

    file.read(reinterpret_cast<char*>(buffer), sizeof(ConfigData));

    file.close();

    return true;
}

bool storeConfigData(const ConfigData data) {
    path cfg_path;

    if (!createPathIfNotExist(cfg_path))
        return false;

    ofstream file{ cfg_path, std::ios::binary };

    file.write(MAGIC_BYTES, sizeof(MAGIC_BYTES));
    file.write(reinterpret_cast<const char*>(&data), sizeof(data));

    file.close();
}

ConfigData config = default_config;
