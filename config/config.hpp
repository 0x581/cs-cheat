#pragma once
#include <atomic>
#define COLOR_NAME_MAX_LENGTH 64
#include "../lib/imgui/imgui.h"




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
    bool boxGlow;
    bool bone;
    bool boxesp;
    bool nameesp;

    ImVec4 colorEnemyVisible;
    ImVec4 colorEnemyHidden;    
    ImVec4 colorTeamVisible;
    ImVec4 colorTeamHidden;     
    ImVec4 colorItem;

};

struct AimBotSettings {
    float pixelRadius;
    float speedFactor;
    bool drawFovCircle;
    bool ignoreteam;
    bool visible;
    ImVec4 fovCircleColor;

    
};

struct FovSettings {
    int desiredFov;
};

struct ConfigData {
    ActiveCheats activeCheats;
    EspSettings esp;
    AimBotSettings aimBot;
    FovSettings fov;
};

template <typename T>
bool loadConfigData(T* buffer, const wchar_t* configName);

template <typename T>
bool storeConfigData(const T data, const wchar_t* configName);

extern ConfigData config;

extern PersistentConfigData persistentConfig;