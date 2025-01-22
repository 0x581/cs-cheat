#pragma once

#include <thread>

#include "../memory.hpp"

#include "../math/math.hpp"
#include "../lib/pch.h"

#include "../shared/defines.hpp"
#include "../shared/sharedVariables.hpp"
#include "../shared/offsets.hpp"

#include "../entityListInteraction.hpp"


void runAimbot(const Memory& mem, const uintptr_t client)
{
	const auto localPlayerPawn = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);
	const auto localPlayerTeam = mem.Read<uint8_t>(localPlayerPawn + offsets::teamNum);

	const auto view_matrix = mem.Read<view_matrix_t>(client + offsets::ViewMatrix);

	for (int i = 0; i < 64; i++) {
		const auto playerPawn = getPlayerPawnPtr(mem, i);

		if (!playerPawn)
		{
			continue;
		}

		const auto playerTeam = mem.Read<uint8_t>(playerPawn + offsets::teamNum);

		if (localPlayerTeam == playerTeam)
		{

			continue;
		}

		const auto health = mem.Read<int>(playerPawn + offsets::health);

		if (health <= 0 || health > 100)
		{

			continue;
		}

		const auto movementServices = mem.Read<intptr_t>(playerPawn + offsets::movementServices);

		if (!movementServices)
		{

			continue;
		}

		const auto movementServices = mem.Read<intptr_t>(playerPawn + offsets::movementServices);

		const auto isCrouched = mem.Read<bool>(movementServices + offsets::inCrouch);

		Vector3 footPos = mem.Read<Vector3>(playerPawn + offsets::OldOrigin);
		Vector3 headPos = { footPos.x, footPos.y, footPos.z + (isCrouched ? 55.f : 75.f) };

		auto screenHeadPos = worldToScreen(headPos, view_matrix);

	}
}
//dont know if i need this
// 
//class Vector
//{
//
//	constexpr Vector(
//		const float x = 0.0f,
//		const float y = 0.0f,
//		const float z = 0.0f) noexcept :
//		x(x), y(y), z(z) {}
//
//	constexpr const Vector& operator-(const Vector& other) const noexcept;
//	constexpr const Vector& operator+(const Vector& other) const noexcept;
//	constexpr const Vector& operator*(const float factor) const noexcept;
//	constexpr const Vector& operator/(const float factor) const noexcept;
//	float x, y, z;
//
//}

void runAimbotLoop(Memory** mem)
{
	while (true)
	{

		std::this_thread::sleep_for(delays::aimbot::sleep);

		const auto client = (*mem)->GetModuleAddress(modules::client);



		const auto localPlayer = (*mem)->Read<uintptr_t>(client + offsets::localPlayerPawn);

		// No local player -> No match currently going on
		if (!localPlayer)
			continue;



		void doAimbot();
		{
			std::this_thread::sleep_for(delays::aimbot2::sleep);

			const auto view_matrix = mem.Read<view_matrix_t>(client + offsets::ViewMatrix);

			std::vector<Vector> playerPositions;

			playerPositions.clear();

		}
		Vector3 findclosest(const std::vector<Vector3> playerpositions);
		{
			if (playerPosition.empty())
			{

				printf("playerPositions vector was empty.\n");

				return(0.0f, 0.0f, 0.0f);
			}
			Vector3 center_off_screen{ (float)GetSystemMetrics(0) / 2, (float)GetSystemMetrics(1) / 2, 0.0f };

			//keep track of the lowest position
			float lowestDistance = 10000;

			//find the index of the new lowest distance in the vector and store it (-1 means there wasn´t one found)
			int index = -1;

			for (int i; i < playerpositions.size(); i++)
			{
				float distance(std::pow(playerPositions[i].x - center_off_screen.x, 2) + std::pow(playerPositions[i].y - center_off_screen.y, 2));

				if (distance < lowestDistance)
				{
					lowestDistance = distance;
					index = i;
				}
			}

			return { playerPositions[index].x , playerPositions[index].y , 0.0f };
		}
		void MoveMouseToPlayer(vector position)
		{
			if (position.x = 0.0 && position.y = 0.0 && position.z = 0.0)
			{
				return;
			}

			Vector center_off_screen{ (float)GetSystemMetrics(0) / 2, (float)GetSystemMetrics(1) / 2, 0.0f };

			auto new_x = position.x - center_off_screen.x;
			auto new_y = position.y - center_off_screen.y;

			mouse_event(MOUSEEVENTF_MOVE, new_x, new_y, 0, 0);
		}
	}
}