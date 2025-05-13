#include "cheats.h"

#include <thread>
#include <windows.h>

#include "config.h"
#include "delays.h"
#include "entityListInteraction.h"
#include "offsets.h"
#include "shared_variables.h"
#include "modules.h"
#include "keybinds.h"
void simulateLeftClick() {
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	std::this_thread::sleep_for(delays::triggerBot::keyPressTime);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

static void runTriggerBot(const Memory& mem, const uintptr_t client) {

	const uintptr_t targetedPawn = getCurrentLookatPlayerPawn(mem);

	if (!targetedPawn)
	{
		return;
	}

	const auto localPlayerPawn = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);
	if (!localPlayerPawn) return;

	const auto localPlayerTeam = mem.Read<PlayerTeam_t>(localPlayerPawn + offsets::teamNum);
	const auto targetTeam = mem.Read<PlayerTeam_t>(targetedPawn + offsets::teamNum);
	const auto targetHealth = mem.Read<int>(targetedPawn + offsets::health);

	if (targetHealth <= 0)
	{
		return;
	}

	if (config.aimBot.ignoreteam && (localPlayerTeam == targetTeam))
	{
		return;
	}
	simulateLeftClick();
	//mem.Write<int>(client + offsets::attack, 65537); // + attack
	//std::this_thread::sleep_for(delays::triggerBot::keyPressTime);
	//mem.Write<int>(client + offsets::attack, 256);   // - attack
}


void runTriggerBotLoop(Memory& mem) {
	while (true)
	{
		if (!config.activeCheats.triggerBot)
		{
			std::this_thread::sleep_for(delays::cheatInactiveWaitTime);
			continue;
		}
		if (!mem.is_valid)
		{
			std::this_thread::sleep_for(delays::memConnectionWaitTime);
			continue;
		}
		std::this_thread::sleep_for(delays::triggerBot::sleep);
		const auto client = mem.GetModuleAddress(modules::client);
		if (!client) continue;
		const auto localPlayerPawn = mem.Read<uintptr_t>(client + offsets::localPlayerPawn);
		if (!localPlayerPawn) continue;
		if (GetAsyncKeyState(keybinds::triggerbot))
			runTriggerBot(mem, client);
	}
}
