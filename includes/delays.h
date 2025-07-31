#pragma once

#include <chrono>

namespace delays
{
    constexpr auto memConnectionWaitTime = std::chrono::seconds(1);
    constexpr auto cheatInactiveWaitTime = std::chrono::milliseconds(10);

    namespace memory
    {
        constexpr auto moduleFetchWaitTime = std::chrono::seconds(1);
    }

    namespace antiFlash
    {
        constexpr auto ms_interval = 10;
        constexpr auto ms_offset = 0;
    }

    namespace bunnyHop
    {
        constexpr auto ms_interval = 10;
        constexpr auto ms_offset = 0;
    }

    namespace fovChanger
    {
        constexpr auto ms_interval = 5;
        constexpr auto ms_offset = 0;
    }

    namespace radarHack
    {
        constexpr auto ms_interval = 1000;
        constexpr auto ms_offset = 0;
    }

    namespace bombTimer
    {
        constexpr auto ms_interval = 50;
        constexpr auto ms_offset = 0;
    }

    namespace esp
    {
        constexpr auto ms_interval = 5;
        constexpr auto ms_offset = 0;
    }

    namespace aimbot
    {
        constexpr auto ms_interval = 15;
        constexpr auto ms_offset = 0;
    }

    namespace triggerBot
    {
        constexpr auto ms_interval = 15;
        constexpr auto ms_offset = 8;
    }

    namespace rageBot = aimbot;
}
