#pragma once

#include <atomic>
#include <utility>
#include <Windows.h>

#include "defines.hpp"

#include "../lib/imgui/imgui.h"

struct EspRenderData {
    bool isValid{ false };
    bool isVisible{ false };
    bool isFriendly{ false };
    char playerName[PLAYER_NAME_LENGTH]{ 0 };
    ImVec2 min{ 0, 0 };
    ImVec2 max{ 0, 0 };
};

struct ActiveCheats {
    std::atomic<bool> bunnyHop{ false };
    std::atomic<bool> antiFlash{ true };
    std::atomic<bool> triggerBot{ true };
    std::atomic<bool> radar{ true };
    std::atomic<bool> fovChanger{ false };
    std::atomic<bool> esp{ true };
    std::atomic<bool> bombHack{ true };
    std::atomic<bool> aimBot{ true };
};

struct FOVData {
    unsigned int desiredFov{ 60 };
};


struct ESPData {
    EspRenderData boxes[64] {};
};

struct BombData {
    bool planted{ false };
    float timeRemaining{ 0.f };
};

extern ActiveCheats activeCheats;

extern FOVData fovData;
extern ESPData espData;
extern BombData bombData;

extern HANDLE consoleOutput;
