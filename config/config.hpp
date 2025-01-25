#pragma once
#include <atomic>

struct ActiveCheats {
    std::atomic<bool> bunnyHop{ false };
    std::atomic<bool> antiFlash{ true };
    std::atomic<bool> triggerBot{ true };
    std::atomic<bool> radar{ true };
    std::atomic<bool> fovChanger{ false };
    std::atomic<bool> esp{ true };
    std::atomic<bool> bombHack{ true };
    std::atomic<bool> aimBot{ true };
};

struct ConfigData {
    ActiveCheats activeCheats;

};
