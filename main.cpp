
#include <thread>
#include <iostream>

#include "lib/pch.h"

#include "memory.hpp"

#include "modules/bunnyHop.hpp"
#include "modules/antiflash.hpp"
#include "modules/triggerbot.hpp"
#include "modules/radarHack.hpp"
#include "modules/fovChanger.hpp"
#include "modules/esp.hpp"
#include "modules/BombTimer.hpp"


static void connectionHandler(Memory** mem) {
    const std::vector<const wchar_t*> requiredModules{ modules::client };

    while (true) {
        if (*mem) {
            DWORD exit_code = 0;

            if (!GetExitCodeProcess((*mem)->getProcessHandle(), &exit_code)) {
                auto error = GetLastError();
                std::cout << "CS2 Memory Connection handler failed to fetch status of CS2: " << error << std::endl;
            }

            if (exit_code != STILL_ACTIVE) {
                std::cout << "Connection to CS2 lost." << std::endl;
                auto temp = *mem;
                *mem = nullptr;

                delete temp;
                temp = nullptr;
            }

            std::this_thread::sleep_for(delays::memConnectionWaitTime);
            continue;
        }

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

    std::thread conn{connectionHandler, &mem};

    std::thread triggerBot{runTriggerBotLoop, &mem};
    std::thread antiFlash{runAntiFlashLoop, &mem};
    std::thread bunnyHop{runBunnyHopLoop, &mem};
    std::thread radarHack{runRadarLoop, &mem};
    std::thread fovChanger{runFovChangerLoop, &mem};
    std::thread esp{runEspLoop, &mem};
    std::thread bombHack{runBombHackLoop, &mem};

    c_overlay overlay;
    while (overlay.msg_loop() && !overlay.ready) {}
    return 0;
}
