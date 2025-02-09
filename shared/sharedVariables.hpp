#pragma once

#include <atomic>
#include <array>
#include <utility>
#include <Windows.h>

#include "defines.hpp"
#include "boneData.hpp"

#include "../lib/imgui/imgui.h"

struct EspRenderData {
    bool isValid{ false };
    bool isVisible{ false };
    bool isFriendly{ false };
    int health{ 0 };
    char playerName[PLAYER_NAME_LENGTH]{ 0 };
    Vector2 min{ 0, 0 };
    Vector2 max{ 0, 0 };
    BoneData bones{};
};

struct ESPData {
    std::chrono::steady_clock::time_point time{ };
    EspRenderData data[64] {};
};

struct BombData {
    bool planted{ false };
    bool canBeDefused{ false };
    float timeRemaining{ 0.f };
};

extern ESPData espData;
extern BombData bombData;

extern HANDLE consoleOutput;
