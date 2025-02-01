#pragma once

#include <Windows.h>
#include <chrono>

typedef unsigned int CEntityIndex;
typedef float GameTime_t;
typedef uint8_t PlayerTeam_t;

class CGlobalVarsBase
{
public:
    float realTime;
    int frameCount;
    float absoluteFrameTime;
    float absoluteFrameStartTimeStdDev;
    int maxClients;
    float intervalPerTick;
private:
    byte pad1[0x1C];
public:
    float currentTime;
    float currentTime2;
private:
    byte pad2[0xC];
public:
    int32_t tickCount;
    float intervalPerTick2;
private:
    byte pad3[0x138];
public:
    uint64_t currentMap;
    uint64_t currentMapName;
};
// Don't trust this struct. I had to manually fix some parts of it already. It should be fine until currentTime2 tho...

constexpr auto PLAYER_NAME_LENGTH = 128;

namespace keybinds
{
    constexpr auto showMenu = VK_PRIOR;
    constexpr auto quickExit = VK_NEXT;

    constexpr auto slowMode = VK_MENU;
    constexpr auto bunnyHop = VK_SPACE;
    constexpr auto triggerbot = VK_RBUTTON;
    constexpr auto aimBot = 0x58;
}

namespace files
{
    constexpr auto persistentConfig = L"persistent";
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
        constexpr auto sleep = std::chrono::milliseconds(50);
    }

    namespace esp
    {
        constexpr auto sleep = std::chrono::milliseconds(0);
    }

    namespace aimbot
    {
        constexpr auto sleep = std::chrono::milliseconds(15);
    }
    namespace aimbot2
    {
        constexpr auto sleep = std::chrono::milliseconds(1);
    }
}

namespace modules
{
    constexpr auto client = L"client.dll";
}

namespace errors
{

}
