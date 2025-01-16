#pragma once

#include <atomic>

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


extern CheatController activeCheats;

extern FOVController fovController;
