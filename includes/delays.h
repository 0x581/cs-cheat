#pragma once

#include <chrono>

namespace delays {
	constexpr auto memConnectionWaitTime = std::chrono::seconds(1);
	constexpr auto cheatInactiveWaitTime = std::chrono::milliseconds(10);

	namespace memory {
		constexpr auto moduleFetchWaitTime = std::chrono::seconds(1);
	}

	namespace antiFlash {
		constexpr auto sleep = std::chrono::milliseconds(10);
	}

	namespace bunnyHop {
		constexpr auto sleep = std::chrono::milliseconds(10);
		constexpr auto keyPressTime = std::chrono::milliseconds(10);
	}

	namespace fovChanger {
		constexpr auto sleep = std::chrono::milliseconds(5);
	}

	namespace radarHack {
		constexpr auto sleep = std::chrono::seconds(1);
	}

	namespace triggerBot {
		constexpr auto sleep = std::chrono::milliseconds(10);
		constexpr auto keyPressDelay = std::chrono::milliseconds(10);
		constexpr auto keyPressTime = std::chrono::milliseconds(10);
	}

	namespace bombHack {
		constexpr auto sleep = std::chrono::milliseconds(50);
	}

	namespace esp {
		constexpr auto sleep = std::chrono::milliseconds(5);
	}

	namespace aimbot {
		constexpr auto sleep = std::chrono::milliseconds(15);
	}
	namespace aimbot2 {
		constexpr auto sleep = std::chrono::milliseconds(1);
	}
}

