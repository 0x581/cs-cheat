
#include <thread>
#include <iostream>

#include "../overlay/overlay.h"

#include "memory/memory.h"
#include "cheats/cheats.h"

#include "exit_codes.h"
#include "delays.h"
#include "files.h"
#include "modules.h"
#include "config.h"


static c_overlay overlay{};

static void connectionHandler(Memory& mem) {
	const std::vector<const wchar_t*> requiredModules{ modules::client };

	while (true)
	{
		if (!mem.is_valid)
		{
			if (mem.connectToProcess(L"cs2.exe", requiredModules))
				std::cout << "Connection to CS2 established." << std::endl;
		}
		else if (!mem.isProcessAlive())
			std::cout << "Connection to CS2 lost." << std::endl;

		std::this_thread::sleep_for(delays::memConnectionWaitTime);
	}
}

static void cleanUp() {
	//overlay.~c_overlay();

	store_persistent_config_data(persistentConfig, files::persistentConfig);
	store_cheat_config_data(config, L"default");
}

static int exitHandler(DWORD fdwCtrlType) {
	switch (fdwCtrlType)
	{
	case CTRL_CLOSE_EVENT:
	case CTRL_C_EVENT:
		overlay.~c_overlay();
		return TRUE;
	}
	return FALSE;
}


int main() {
	std::cout << "Running main..." << std::endl;

	load_persistent_config_data(&persistentConfig, files::persistentConfig);
	load_cheat_config_data(&config, L"default");

	SetConsoleCtrlHandler(exitHandler, TRUE);

	Memory mem{};

	std::thread conn{ connectionHandler, std::ref(mem) };
	conn.detach();

	std::thread cheats_thread{ run_all_cheats_loop, std::ref(mem) };
	cheats_thread.detach();

	std::thread rageaimBot{ runRageAimbotLoop, std::ref(mem) };
	rageaimBot.detach();

	while (overlay.msg_loop() && !overlay.ready_to_close) {}

	cleanUp();

	return exit_codes::SUCCESS;
}
