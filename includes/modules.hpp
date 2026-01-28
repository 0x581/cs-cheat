#pragma once

#include "CheatBase/memory.hpp"

#include <array>

namespace modules
{
    static constexpr const int client = 0;
}

static constexpr const std::array requiredModules{
    ModuleEntry(modules::client, L"client.dll"),
};
