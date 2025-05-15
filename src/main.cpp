
#include <thread>
#include <iostream>

#include "../overlay/overlay.h"

#include "memory/memory.h"
#include "cheats/cheats.h"

#include "delays.h"
#include "files.h"
#include "modules.h"
#include "config.h"


static c_overlay overlay{};

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

	std::thread cheats_thread{ run_cheat_loop, std::ref(mem) };
	cheats_thread.detach();


	while (overlay.msg_loop() && !overlay.ready_to_close) {}

	cleanUp();
}
