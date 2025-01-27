
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

        storeConfigData(sharedConfig, L"common");
        return TRUE;
    }
    return FALSE;
}


int main()
{
    loadConfigData(&persistentConfig, files::persistentConfig);

    SetConsoleCtrlHandler(exitHandler, TRUE);

    Memory *mem = nullptr;

    std::thread conn{connectionHandler, &mem};

    std::thread triggerBot{runTriggerBotLoop, &mem};
    std::thread antiFlash{runAntiFlashLoop, &mem};
    std::thread bunnyHop{runBunnyHopLoop, &mem};
    std::thread radarHack{runRadarLoop, &mem};
    std::thread fovChanger{runFovChangerLoop, &mem};
    std::thread esp{runEspLoop, &mem};
    std::thread bombHack{runBombHackLoop, &mem};
    std::thread aimBot{runAimBotLoop, &mem};

    while (overlay.msg_loop() && !overlay.ready_to_close) {}

    overlay.~c_overlay();
}
