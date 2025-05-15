#pragma once


#define PERSISTENT_CONFIG_EXT L".cfg"

namespace files {
	constexpr auto folder = L"%localappdata%/Merkur/CS2";

	constexpr auto imguiIni = L"imgui.ini";
	constexpr auto persistentConfig = L"persistent" PERSISTENT_CONFIG_EXT;
}
