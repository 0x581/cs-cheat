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
    ActiveCheats activeCheats;
    EspSettings esp;
};

struct SharedConfigData {
    MainWindowSettings mainWindow;
    BombSettings bombWindow;
};

static constexpr const ConfigData default_config{
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
    .esp {.selectedColor{0}, .renderFriendlies = false },
};

static constexpr const SharedConfigData default_shared_config{
    .mainWindow {.windowPosition = ImVec2(50, 100) },
    .bombWindow {.windowPosition = ImVec2(50, 50)},
};

template <typename T>
bool loadConfigData(T* buffer, const wchar_t* configName);

template <typename T>
bool storeConfigData(const T data, const wchar_t* configName);

extern ConfigData config;

extern SharedConfigData sharedConfig;