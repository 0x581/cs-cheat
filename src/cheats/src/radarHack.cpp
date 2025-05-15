#include "../cheats.h"

#include <offsets.h>


static void runRadarHack(const Memory& mem, const CheatData& cheatData) {
	for (size_t i = 0; i < 64; i++)
	{
		const auto playerEntity = cheatData.playerEntities[i];

		if (!playerEntity.isValid)
			continue;

		const auto entityPawnPtr = playerEntity.pawnPtr;

		if (!entityPawnPtr)
		{
			std::cout << "Entity pawn pointer is null." << std::endl;
			continue;
		}

		mem.Write<bool>(entityPawnPtr + offsets::entitySpottedState + offsets::spotted, true);
	}
}
