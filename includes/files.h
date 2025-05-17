#pragma once
#include <string>


#define CONFIG_EXT ".cfg"

namespace files {
	constexpr const std::string_view folder = "%localappdata%/Merkur/CS2";

	constexpr const std::string_view configExtension{ CONFIG_EXT };
	constexpr const std::string_view imguiIni{ "imgui.ini" };
	constexpr const std::string_view persistentConfig{ "persistent" };
	constexpr const std::string_view defaultConfig{ "default" };
}

namespace driver_files {
	constexpr const std::string_view driverName = "driver.sys";
	constexpr const std::string_view kdmapper = "kdmapper_Release.exe";
}