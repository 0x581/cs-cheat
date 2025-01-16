
#include <thread>
#include <iostream>

#include "memory.hpp"
#include "./math/math.hpp"

#include "modules/bunnyHop.hpp"
#include "modules/antiflash.hpp"
#include "modules/triggerbot.hpp"
#include "modules/radarHack.hpp"
#include "modules/fovChanger.hpp"
#include "modules/esp.hpp"

#include "./lib/pch.h"

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

    //std::thread fovChanger{runFovChangerLoop, &mem};
    //const auto client = mem->GetModuleAddress(L"client.dll");
    //const auto localPlayer = mem->Read<uintptr_t>(client + offsets::localPlayerPawn);
    //int localTeam = mem->Read<int>(client + offsets::teamNum);
    //const auto entList = mem->Read<uintptr_t>(client + offsets::entityList);
    //view_matrix_t view_matrix = mem->Read<view_matrix_t>(client + offsets::ViewMatrix);



    //for (int i = 0; i < 64; i++) {
    //    uintptr_t listEntry = mem->Read<uintptr_t>(entList + (8 * (i & 0x7FFF) >> 9) + 16);
    //    if (!listEntry)
    //        continue;

    //    const auto entController = mem->Read<uintptr_t>(listEntry + 120 * (i & 0x1FF));
    //    if (!entController)
    //        continue;

    //    const auto entControllerPawn = mem->Read<uintptr_t>(entController + offsets::pawnOfController);
    //    if (!entControllerPawn)
    //        continue;

    //    const auto entPawn = mem->Read<uintptr_t>(listEntry + 120 * (entControllerPawn & 0x1FF));
    //    if (!entPawn)
    //        continue;

    //    int entTeam = mem->Read<int>(entPawn + offsets::teamNum);
    //    if (entTeam == localTeam)
    //        continue;

    //    int health = mem->Read<int>(entPawn + offsets::health);
    //    if (health <= 0 || health > 100)
    //        continue;

    //    Vector3 origin = mem->Read<Vector3>(entPawn + offsets::OldOrigin);

    //    Vector3 head = { origin.x, origin.y, origin.z + 75.4f };

    //    Vector3 localOrigin = mem->Read<Vector3>(localPlayer + offsets::OldOrigin);

    //    Vector3 head1 = head;
    //    head1.z = head.z + 10.f;

    //    Vector3 screenFeetPos = origin.WorldToScreen(view_matrix);
    //    Vector3 screenHeadPos = head1.WorldToScreen(view_matrix);

    //    float headHeight = (screenFeetPos.y - screenHeadPos.y) / 8;

    //    float height = screenFeetPos.y - screenHeadPos.y;
    //    float width = height / 2.4F;
    //    float value = 2.0f; // Mit `f` für float.

    //    // Berechne die obere linke Ecke der Box
    //    ImVec2 boxPos = ImVec2((float)(screenHeadPos.x - width / 2.0f), (float)screenHeadPos.y);


    //    ImU32 red = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    //    //overlay.box(boxPos, width, height, red, value);

    //}
    
    c_overlay overlay;
    while (overlay.msg_loop() && !overlay.ready) {}

    return 0;
}
