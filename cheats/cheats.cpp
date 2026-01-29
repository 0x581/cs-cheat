#include "cheats.hpp"

#include <thread>

#include "CheatBase/memory.hpp"

#include "modules.hpp"
#include "delays.hpp"
#include "entity_system.hpp"
#include "shared_variables.hpp"

#include "check_cheat_should_run.hpp"
#include "src/cheat_entries.hpp"

static void run_all_cheats(const Memory &mem, const CheatData &cheat_data, const size_t iteration)
{
    if (shouldAimBotRun(iteration))
        runAimBot(cheat_data);

    if (shouldTriggerBotRun(iteration))
        runTriggerBot(cheat_data);

    if (shouldEspRun(iteration))
        runEsp(cheat_data);

    if (shouldBunnyHopRun(iteration))
        runBunnyHop();

    if (shouldBombTimerRun(iteration))
        runBombTimer(cheat_data);

    if (shouldAntiFlashRun(iteration))
        runAntiFlash(mem, cheat_data);

    if (shouldFovChangerRun(iteration))
        runFovChanger(mem, cheat_data);

    if (shouldRadarHackRun(iteration))
        runRadarHack(mem, cheat_data);

    if (shouldRageBotRun(iteration))
        runRageAimbot(cheat_data);
}

void run_cheat_loop(bool &terminate)
{
    memory.setRequiredModulesList(requiredModules);
    memory.connectToProcess(L"cs2.exe");

    auto entity_list = new EntityData();
    auto cheat_data = new CheatData();

    auto lastTime = std::chrono::high_resolution_clock::now();

    for (size_t iteration = 0; !terminate; ++iteration)
    {
        if (!memory.reconnectIfNeeded())
        {
            std::this_thread::sleep_for(delays::memConnectionWaitTime);
            lastTime = std::chrono::high_resolution_clock::now();
            continue;
        }

        lastTime += std::chrono::milliseconds(1);

        std::this_thread::sleep_until(lastTime);

        if (!read_entity_data(memory, *entity_list) || !read_cheat_data(memory, *entity_list, *cheat_data))
        {
            renderData.isValid = false;
            continue;
        }

        run_all_cheats(memory, *cheat_data, iteration);

        renderData.lastCaptureTime = lastTime;
        renderData.isValid = true;
    }

    delete cheat_data;
    delete entity_list;

    std::cout << "Cheat thread terminated." << std::endl;
}
