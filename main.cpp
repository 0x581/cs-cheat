
#include <thread>
#include <iostream>

#include "memory.hpp"

#include "modules/bunnyHop.hpp"
#include "modules/antiflash.hpp"
#include "modules/triggerbot.hpp"
#include "modules/radarHack.hpp"

#include "./lib/pch.h"

void connectToCS2(Memory** mem) {
    const std::vector<const wchar_t*> requiredModules{ modules::client };

    while (!*mem) {
        Memory* temp = new Memory(L"cs2.exe", requiredModules);

        if (!temp->getProcessId() || !temp->getProcessHandle())
        {
            delete temp;
            temp = nullptr;
        }
        else {
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
    std::thread radar{runRadarLoop, &mem};

    while (mem == nullptr)
        std::this_thread::sleep_for(delays::memConnectionWaitTime);

    const auto client = mem->GetModuleAddress(modules::client);

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        const auto localPlayer = mem->Read<uintptr_t>(client + offsets::localPlayerPawn);

        // No local player -> No match currently going on
        if (!localPlayer)
            continue;

        const auto currentController = getCurrentLookatPlayerController(*mem);


        if (!currentController)
            continue;

        char name[129]{ 0 };

        mem->ReadBytes(currentController + offsets::playerName, (byte*)name, 128);

        for (size_t i = 0; i < 128; i++)
        {
            if (name[i] == 0x0d)
                name[i] = '?';
        }

        const auto lifeState = mem->Read<uint8_t>(localPlayer + offsets::teamNum);

        std::cout << "Name: " << name << std::endl;
        std::cout << "Life state: " << (int)lifeState << std::endl;


        //for (size_t i = 0; i < 64; i++)
        //{
        //    const auto entityController = getControllerPtr(*mem, i);

        //    if (!entityController)
        //        continue;

        //    char name[129]{ 0 };

        //    mem->ReadBytes(entityController + offsets::playerName, (byte*)name, 128);

        //    for (size_t i = 0; i < 128; i++)
        //    {
        //        if (name[i] == 0x0d)
        //            name[i] = '?';
        //    }

        //    std::cout << "Name: " << name << std::endl;
        //}
        //std::cout << std::endl;
    }


    c_overlay overlay;
    while (overlay.msg_loop() && !overlay.ready) {}

    return 0;
}
