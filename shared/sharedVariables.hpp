#pragma once

#include <atomic>

#include "../lib/imgui/imgui.h"
#include <utility>
#include <Windows.h>


struct EspRenderData {
    ImVec2 min;
    ImVec2 max;
    std::string name;
};

struct CheatController {
    std::atomic<bool> bunnyHop{ false };
    std::atomic<bool> antiFlash{ true };
    std::atomic<bool> triggerBot{ true };
    std::atomic<bool> radar{ true };
    std::atomic<bool> fovChanger{ false };
    std::atomic<bool> esp{ false };
    std::atomic<bool> bombHack{ true };
};

struct FOVData {
    unsigned int desiredFov{ 60 };
};


struct ESPData {
    std::pair<ImVec2, ImVec2> *boxes[64] {};
};

struct BombData {
    bool planted{ false };
    float timeRemaining{ 0.f };
};

extern CheatController activeCheats;

extern FOVData fovData;
extern ESPData espData;
extern BombData bombData;

extern HANDLE consoleOutput;
