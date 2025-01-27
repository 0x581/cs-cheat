
#include <thread>
#include <iostream>

#include "lib/pch.h"

#include "memory.hpp"

#include "cheats/cheats.hpp"
#include <csignal>

#include "config/config.hpp"


static c_overlay overlay{};

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
    loadConfigData(&sharedConfig, L"common");

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
