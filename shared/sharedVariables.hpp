#pragma once

#include <atomic>

#include "../lib/imgui/imgui.h"
#include <utility>
#include <Windows.h>


struct CheatController {
    std::atomic<bool> bunnyHop{ false };
    std::atomic<bool> antiFlash{ false };
    std::atomic<bool> triggerBot{ false };
    std::atomic<bool> radar{ false };
    std::atomic<bool> fovChanger{ true };
    std::atomic<bool> esp{ true };    
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
