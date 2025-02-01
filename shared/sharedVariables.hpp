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

struct ESPData {
    std::chrono::steady_clock::time_point time{ };
    EspRenderData boxes[64] {};
};

struct BombData {
    bool planted{ false };
    float timeRemaining{ 0.f };
};

extern ESPData espData;
extern BombData bombData;

extern HANDLE consoleOutput;
