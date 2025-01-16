#pragma once

#include <thread>

#include "../shared/defines.hpp"
#include "../shared/sharedVariables.hpp"
#include "../shared/offsets.hpp"

#include "../memory.hpp"

#include "../math/math.hpp"
#include "../lib/pch.h"


#include "../entityListInteraction.hpp"


void runEsp(const Memory& mem, const uintptr_t client)
{
    const auto localPlayer = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);
    int localTeam = mem.Read<int>(client + offsets::teamNum);
    const auto entList = mem.Read<uintptr_t>(client + offsets::entityList);
    view_matrix_t view_matrix = mem.Read<view_matrix_t>(client + offsets::ViewMatrix);
    
    

    for (int i = 0; i < 64; i++) {
        uintptr_t listEntry = mem.Read<uintptr_t>(entList + (8 * (i & 0x7FFF) >> 9) + 16);
        if (!listEntry)
            continue;

        const auto player = mem.Read<uintptr_t>(listEntry + 120 * (i & 0x1FF));
        if (!player)
            continue;

        uintptr_t playerPawn = mem.Read<uintptr_t>(player + offsets::localPlayerPawn);

        uintptr_t listEntry2 = mem.Read<uintptr_t>(entList + 0x8 * ((playerPawn  & 0x7FFF) >> 9) + 16);
        if (!listEntry2)
            continue;


        uintptr_t pcsplayerPawn = mem.Read<uintptr_t>(listEntry2 + 120 * (playerPawn & 0x1FF));


        int health = mem.Read<int>(pcsplayerPawn + offsets::health);
        if (health <= 0 || health > 100)
            continue;

        Vector3 origin = mem.Read<Vector3>(pcsplayerPawn + offsets::OldOrigin);

        Vector3 head = { origin.x, origin.y, origin.z + 75.f };

        Vector3 localOrigin = mem.Read<Vector3>(localPlayer + offsets::OldOrigin);

       

        Vector3 screenPos = origin.WorldToScreen(view_matrix);
        Vector3 screenHead = head.WorldToScreen(view_matrix);

        float height = (screenPos.y - screenHead.y) / 8;

        float height = screenPos.y - screenHead.y;
        float width = height / 2.4F;
       
        DrawRect(
            screenHead.x - width /2,
            screenHead.y,
            width,
            height,
            red,
            1.5
        );

    }
}

void runEspLoop(Memory** mem)
{

	while (true)
	{
		if (activeCheats.esp.load())
		{
			if (!*mem) {
				std::this_thread::sleep_for(delays::memConnectionWaitTime);
				continue;
			}

			std::this_thread::sleep_for(delays::radarHack::sleep);

			const auto client = (*mem)->GetModuleAddress(modules::client);

			const auto localPlayer = (*mem)->Read<uintptr_t>(client + offsets::localPlayerPawn);

			// No local player . No match currently going on
			if (!localPlayer)
				continue;

            runEsp(**mem, client);  


		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
}

void DrawRect(int x, int y, int w, int h, RGB color, int thickness)
{
    ImGui::GetBackgroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), Color(color), 0, 0, thickness);
}
typedef struct
{
    ImU32 R;
    ImU32 G;
    ImU32 B;

} RGB;


ImU32 Color(RGB color)
{
    return IM_COL32(color.R, color.G, color.R, 255);
}

RGB red = { 255, 0, 0 };