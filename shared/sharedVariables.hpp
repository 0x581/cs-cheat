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
    bool bunnyHop{ false };
    bool antiFlash{ true };
    bool triggerBot{ true };
    bool radar{ true };
    bool fovChanger{ false };
    bool esp{ true };
    bool bombHack{ true };
    bool aimBot{ true };
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
