
#include <thread>
#include <iostream>

#include "memory.hpp"

#include "modules/bunnyHop.hpp"
#include "modules/antiflash.hpp"
#include "modules/triggerbot.hpp"
#include "modules/radarHack.hpp"
#include "modules/fovChanger.hpp"

#include "./lib/pch.h"

void connectToCS2(Memory** mem) {
    const std::vector<const wchar_t*> requiredModules{ modules::client };

    while (!*mem) {
        Memory* temp = new Memory(L"cs2.exe", requiredModules);

        if (!temp->getProcessId() || !temp->getProcessHandle())
        {
            delete temp;
            temp = nullptr;
        } else {
            std::cout << "Connection to CS2 established." << std::endl;
            *mem = temp;
        }

        std::this_thread::sleep_for(delays::memConnectionWaitTime);
    }
}

int main()
{
    Memory *mem = nullptr;

    std::thread conn{connectToCS2, &mem};

    std::thread triggerBot{runTriggerBotLoop, &mem};
    std::thread antiFlash{runAntiFlashLoop, &mem};
    std::thread bunnyHop{runBunnyHopLoop, &mem};
    std::thread radarHack{runRadarLoop, &mem};
    //std::thread fovChanger{runFovChangerLoop, &mem};

    c_overlay overlay;
    while (overlay.msg_loop() && !overlay.ready) {}

    return 0;
}
