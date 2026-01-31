#pragma once

#include "CheatBase/config_manager.hpp"

#include "CheatBase/toggleable_keybind.hpp"

#include "imgui.h"
#include "json.h"

struct PersistentConfigData
{
    struct MainWindowSettings
    {
        ImVec2 position = ImVec2(50, 100);
        ImVec2 size = ImVec2(650, 500);
        float scale = 1.f;
    } mainWindow;

    struct BombSettings
    {
        ImVec2 position = ImVec2(50, 50);
        float scale = 1.f;
    } bombWindow;

    int targetFPS = 0;
    bool showFPS = false;

    float espFontScale = 1.f;
};

struct ConfigData
{
    struct ActiveCheats
    {
        bool aimBot = false;
        bool antiFlash = false;
        bool bombTimer = false;
        bool bunnyHop = false;
        bool esp = false;
        bool fovChanger = false;
        bool radar = false;
        bool triggerBot = false;
        bool rageBot = false;
    } activeCheats;

    struct EspSettings
    {
        bool renderFriendlies = false;
        bool boxGlow = false;
        bool bone = false;
        bool box = false;
        bool name = false;
        float font_scale = 1.f;
        bool health = false;
        float boneWidth = 1.f;

        ImVec4 colorEnemyVisible = {1.0f, 0.0f, 0.0f, 1.0f};
        ImVec4 colorEnemyHidden = {1.0f, 0.5f, 0.0f, 1.0f};
        ImVec4 colorTeamVisible = {0.0f, 1.0f, 0.0f, 1.0f};
        ImVec4 colorTeamHidden = {0.0f, 0.5f, 1.0f, 1.0f};
        ImVec4 colorItem = {1.0f, 1.0f, 0.0f, 1.0f};
    } esp;

    struct AimBotSettings
    {
        float pixelRadius = 100.f;
        float speedFactor = 1.f;
        bool drawFovCircle = true;
        bool ignoreTeam = true;
        bool checkVisible = false;
        ImVec4 fovCircleColor = {1.0f, 0.0f, 1.0f, 0.7f};
        int hitbox = 0;
        int shotDelay = 30;
    } aimBot;

    struct FovSettings
    {
        int desiredFov = 90;
    } fov;

    struct RcsSettings
    {
        bool Rcs;
        float OldPunchAngleX = 0.0f;
        float OldPunchAngleY = 0.0f;
    } rcs;

    struct RageBotSettings
    {
        float pixelRadius = 100;
        float speedFactor = 1.f;
        bool checkVisible = false;
        int hitbox = 0;
        int shotDelay = 30;
    } rageBot;

    struct DevSettings
    {
        bool devMode = false;
    } devSettings;

    struct KeyBinds
    {
        int showMenu = VK_PRIOR;
        int quickExit = VK_NEXT;

        ToggleableKeybind bunnyHop = {VK_SPACE};
        ToggleableKeybind triggerBot = {VK_RBUTTON};
        ToggleableKeybind aimBot = {VK_RBUTTON};
        ToggleableKeybind rageBot = {VK_RBUTTON};
    } keybinds;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(PersistentConfigData::MainWindowSettings, position, size, scale)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(PersistentConfigData::BombSettings, position, scale)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(PersistentConfigData, mainWindow, bombWindow, targetFPS, showFPS, espFontScale)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ConfigData::ActiveCheats, aimBot, antiFlash, bombTimer, bunnyHop, esp, fovChanger, radar, triggerBot, rageBot)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ConfigData::EspSettings, renderFriendlies, boxGlow, bone, box, name, font_scale, health, boneWidth, colorEnemyVisible, colorEnemyHidden, colorTeamVisible, colorTeamHidden, colorItem)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ConfigData::AimBotSettings, pixelRadius, speedFactor, drawFovCircle, ignoreTeam, checkVisible, fovCircleColor, hitbox, shotDelay)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ConfigData::FovSettings, desiredFov)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ConfigData::RcsSettings, Rcs, OldPunchAngleX, OldPunchAngleY)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ConfigData::RageBotSettings, pixelRadius, speedFactor, checkVisible, hitbox, shotDelay)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ConfigData::DevSettings, devMode)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ConfigData::KeyBinds, showMenu, quickExit, bunnyHop, triggerBot, aimBot, rageBot)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ConfigData, activeCheats, esp, aimBot, fov, rcs, rageBot, devSettings, keybinds)

using ConfigManagerInstance = ConfigManager<PersistentConfigData, ConfigData>;

extern ConfigManagerInstance cfg;
