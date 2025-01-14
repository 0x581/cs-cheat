#pragma once

#include <atomic>

struct CheatController
{
    std::atomic<bool> bunnyHop{false};
    std::atomic<bool> antiFlash{true};
    std::atomic<bool> triggerBot{true};
    std::atomic<bool> radar{true};
    std::atomic<bool> fovChanger{true};
};

struct FOVController
{
    std::atomic<int> desiredFov{ 90 };
};

extern CheatController activeCheats;

extern FOVController fovController;
