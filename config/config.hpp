#pragma once
#include <atomic>

#include "../lib/imgui/imgui.h"

constexpr auto COLOR_NAME_MAX_LENGTH = 32;


struct MainWindowSettings {
    ImVec2 windowPosition;
};

struct ActiveCheats {
    bool aimBot;
    bool antiFlash;
    bool bombHack;
    bool bunnyHop;
    bool esp;
    bool fovChanger;
    bool radar;
    bool triggerBot;
};

struct EspSettings {
    char selectedColor[COLOR_NAME_MAX_LENGTH];
    bool renderFriendlies;
};

struct BombSettings {
    ImVec2 windowPosition;
};

struct ConfigData {
    MainWindowSettings mainWindowSettings;
    ActiveCheats activeCheats;
    EspSettings espSettings;
    BombSettings bombSettings;
};

static constexpr const ConfigData default_config{
    .mainWindowSettings {.windowPosition = ImVec2(500, 500) },
    .activeCheats {
        .aimBot = true,
        .antiFlash = true,
        .bombHack = true,
        .bunnyHop = false,
        .esp = true,
        .fovChanger = false,
        .radar = true,
        .triggerBot = true,
    },
    .espSettings {.selectedColor{0}, .renderFriendlies = false },
    .bombSettings {.windowPosition = ImVec2(200, 200)}
};

bool loadConfigData(ConfigData* buffer, const wchar_t* configName);
bool storeConfigData(const ConfigData data, const wchar_t* configName);

extern ConfigData config;
