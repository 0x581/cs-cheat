#pragma once
#include <array>

typedef int ModuleID;

namespace modules {
	static constexpr const ModuleID client = 0;
}

static constexpr const auto requiredModules = std::to_array(
	{
		std::make_pair(modules::client, L"client.dll")
	}
);
