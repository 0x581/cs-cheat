#pragma once

#include <atomic>

#include "../lib/imgui/imgui.h"
#include <utility>
#include <Windows.h>


struct CheatController
{
    std::atomic<bool> bunnyHop{ false };
    std::atomic<bool> antiFlash{ false };
    std::atomic<bool> triggerBot{ false };
    std::atomic<bool> radar{ false };
    std::atomic<bool> fovChanger{ true };
    std::atomic<bool> esp{ false };    
    std::atomic<bool> bombHack{ true };
};

struct FOVController
{
    std::atomic<unsigned int> desiredFov{ 60 };
};


struct ESPController
{
    std::pair<ImVec2, ImVec2> *boxes[64] {};
};


extern CheatController activeCheats;

extern FOVController fovController;

extern ESPController espController;

extern HANDLE consoleOutput;
