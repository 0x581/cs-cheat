
#include <thread>
#include <iostream>

#include "lib/pch.h"

#include "memory.hpp"

#include "shared/defines.hpp"

#include "cheats/cheats.hpp"
#include <csignal>

#include "config/config.hpp"


static c_overlay overlay{};

static void connectionHandler(Memory &mem) {
    const std::vector<const wchar_t*> requiredModules{ modules::client };

    while (true) {
        if (!mem.isValid())
        {
            if (mem.connectToProcess(L"cs2.exe", requiredModules))
                std::cout << "Connection to CS2 established." << std::endl;

            }
        else if (!mem.processIsAlive())
        {
            mem.invalidate();
                std::cout << "Connection to CS2 lost." << std::endl;
            }

            std::this_thread::sleep_for(delays::memConnectionWaitTime);
        }
        }

static int exitHandler(DWORD fdwCtrlType) {
    switch (fdwCtrlType)
    {
    case CTRL_CLOSE_EVENT:
    case CTRL_C_EVENT:
        overlay.~c_overlay();

        storeConfigData(persistentConfig, files::persistentConfig);
        return TRUE;
    }
    return FALSE;
}


int main()
{
    loadConfigData(&persistentConfig, files::persistentConfig);

    SetConsoleCtrlHandler(exitHandler, TRUE);

    Memory mem{};

    std::thread conn{connectionHandler, std::ref(mem)};

    std::thread triggerBot{runTriggerBotLoop, std::ref(mem) };
    std::thread antiFlash{runAntiFlashLoop, std::ref(mem) };
    std::thread bunnyHop{runBunnyHopLoop, std::ref(mem) };
    std::thread radarHack{runRadarLoop, std::ref(mem) };
    std::thread fovChanger{runFovChangerLoop, std::ref(mem) };
    std::thread esp{runEspLoop, std::ref(mem) };
    std::thread bombHack{runBombHackLoop, std::ref(mem) };
    std::thread aimBot{runAimBotLoop, std::ref(mem) };

    while (overlay.msg_loop() && !overlay.ready_to_close) {}

    overlay.~c_overlay();
}
