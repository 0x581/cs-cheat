#include "cheats.hpp"

#include <thread>

#include "../shared/defines.hpp"
#include "../config/config.hpp"
#include "../shared/offsets.hpp"

#include "../entityListInteraction.hpp"

#include "../math/math.hpp"


static void runAimBot(const Memory& mem, const uintptr_t client)
{
	const auto localPlayerPawn = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);
	const auto localPlayerTeam = mem.Read<PlayerTeam_t>(localPlayerPawn + offsets::teamNum);

	const auto view_matrix = mem.Read<view_matrix_t>(client + offsets::ViewMatrix);

	Vector2 screenCenter{ (float)GetSystemMetrics(0) / 2.f, (float)GetSystemMetrics(1) / 2.f };

	auto lowestDistance = HUGE_VAL;
	uintptr_t closestPawnPtr = 0;
	Vector3 closestPoint{ 0 };


	for (int i = 0; i < 64; i++) {
		const auto playerPawn = getPlayerPawnPtr(mem, i);

		if (!playerPawn)
			continue;

		const auto playerTeam = mem.Read<PlayerTeam_t>(playerPawn + offsets::teamNum);

		if (localPlayerTeam == playerTeam)
			continue;

		const auto health = mem.Read<int>(playerPawn + offsets::health);

		if (health <= 0 || health > 100)
			continue;

		const auto movementServices = mem.Read<intptr_t>(playerPawn + offsets::movementServices);

		if (!movementServices)
			continue;

		const auto crouchAmount = mem.Read<float>(movementServices + offsets::crouchAmount);

		Vector3 footPos = mem.Read<Vector3>(playerPawn + offsets::OldOrigin);
		Vector3 headPos = { footPos.x, footPos.y, footPos.z + 65.f - (crouchAmount * 17.f) };

		auto screenHeadPos = worldToScreen(headPos, view_matrix);

		if (screenHeadPos.z == 0.f)
			continue;

		double distance(std::pow(screenHeadPos.x - screenCenter.x, 2) + std::pow(screenHeadPos.y - screenCenter.y, 2));

		if (distance >= lowestDistance)
			continue;


		lowestDistance = distance;
		closestPawnPtr = playerPawn;
		closestPoint = screenHeadPos;
	}

	if (lowestDistance == INFINITY)
		return;

	const auto radius = config.aimBot.pixelRadius;

	const auto maxDistance = 2.f * radius * radius;

	if (lowestDistance > maxDistance)
		return;

	const auto diff_x = closestPoint.x - screenCenter.x;
	const auto diff_y = closestPoint.y - screenCenter.y;

	const auto speedFactor = config.aimBot.speedFactor;

	auto new_x = (DWORD)(diff_x * speedFactor);
	auto new_y = (DWORD)(diff_y * speedFactor);

	mouse_event(MOUSEEVENTF_MOVE, new_x, new_y, 0, 0);
}

void runAimBotLoop(Memory& mem)
{
	while (true)
	{
		if (!config.activeCheats.aimBot)
		{
			std::this_thread::sleep_for(delays::cheatInactiveWaitTime);
			continue;
		}

		if (!mem.isValid()) {
			std::this_thread::sleep_for(delays::memConnectionWaitTime);
			continue;
		}

		std::this_thread::sleep_for(delays::aimbot::sleep);

		const auto client = mem.GetModuleAddress(modules::client);

		const auto localPlayer = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);

		// No local player -> No match currently going on
		if (!localPlayer)
			continue;

		if (GetAsyncKeyState(keybinds::aimBot))
			runAimBot(mem, client);
	}
}