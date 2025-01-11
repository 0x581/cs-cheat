#pragma once

#include <atomic>

struct CheatController
{
    std::atomic<bool> bunnyHop{true};
    std::atomic<bool> antiFlash{true};
    std::atomic<bool> triggerBot{true};
};

extern CheatController activeCheats;
