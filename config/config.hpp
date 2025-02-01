#pragma once
#include <atomic>

#include "../lib/imgui/imgui.h"

constexpr auto COLOR_NAME_MAX_LENGTH = 32;


struct MainWindowSettings {
    ImVec2 windowPosition;
};

struct BombSettings {
    ImVec2 windowPosition;
};

struct PersistentConfigData {
    MainWindowSettings mainWindow;
    BombSettings bombWindow;
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

struct AimBotSettings {
    float pixelRadius;
    float speedFactor;
};

struct ConfigData {
    ActiveCheats activeCheats;
    EspSettings esp;
    AimBotSettings aimBot;
};

template <typename T>
bool loadConfigData(T* buffer, const wchar_t* configName);

template <typename T>
bool storeConfigData(const T data, const wchar_t* configName);

extern ConfigData config;

extern PersistentConfigData persistentConfig;