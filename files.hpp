#pragma once

#include <string>

namespace files
{
    constexpr const std::string_view configExtension{".cfg"};

    constexpr const std::string_view persistentConfig{"persistent"};
    constexpr const std::string_view defaultConfig{"default"};
}

namespace driver_files
{
    constexpr const std::string_view folder = "%localappdata%/Merkur/Drivers";

    constexpr const std::string_view driverName = "driver.sys";
    constexpr const std::string_view kdmapper = "kdmapper_Release.exe";
}