#pragma once

#include <atomic>

struct CheatController
{
    std::atomic<bool> bunnyHop{ false };
    std::atomic<bool> antiFlash{ false };
    std::atomic<bool> triggerBot{ false };
    std::atomic<bool> radar{ false };
    std::atomic<bool> fovChanger{ false };
    std::atomic<bool> esp{ false };
};

struct FOVController
{
    std::atomic<int> desiredFov{ 90 };
};


extern CheatController activeCheats;

extern FOVController fovController;
