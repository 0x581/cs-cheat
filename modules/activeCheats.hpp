#pragma once

#include <atomic>

struct CheatController
{
    std::atomic<bool> bunnyHop{true};
    std::atomic<bool> antiFlash{true};
    std::atomic<bool> triggerBot{true};
    std::atomic<bool> radar{true};
};

extern CheatController activeCheats;
