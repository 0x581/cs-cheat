#pragma once

#include <Windows.h>

typedef unsigned int CEntityIndex;
typedef float GameTime_t;


namespace keybinds
{
    constexpr auto showMenu = VK_PRIOR;
    constexpr auto quickExit = VK_NEXT;

    constexpr auto slowMode = VK_MENU;
    constexpr auto bunnyHop = VK_SPACE;
    constexpr auto triggerbot = VK_RBUTTON;
}

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
        constexpr auto sleep = std::chrono::milliseconds(10);
    }

    namespace bunnyHop
    {
        constexpr auto sleep = std::chrono::milliseconds(10);
        constexpr auto keyPressTime = std::chrono::milliseconds(10);
    }

    namespace fovChanger
    {
        constexpr auto sleep = std::chrono::milliseconds(5);
    }

    namespace radarHack
    {
        constexpr auto sleep = std::chrono::seconds(1);
    }

    namespace triggerBot
    {
        constexpr auto sleep = std::chrono::milliseconds(10);
        constexpr auto keyPressDelay = std::chrono::milliseconds(10);
        constexpr auto keyPressTime = std::chrono::milliseconds(10);
    }

    namespace bombHack
    {
        constexpr auto sleep = std::chrono::milliseconds(10);
    }

    namespace esp
    {
        constexpr auto sleep = std::chrono::milliseconds(10);
    }
}

namespace modules
{
    constexpr auto client = L"client.dll";
}

namespace errors
{

}
