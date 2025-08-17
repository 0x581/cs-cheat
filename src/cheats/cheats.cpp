#include "cheats.h"

#include <thread>

#include <delays.h>
#include <entity_system.h>

#include "check_cheat_should_run.h"
#include "src/cheat_entries.h"

#include "../memory/memory.h"
#include <shared_variables.h>

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

static bool check_connection(Memory &mem)
{
    const auto previous = mem.is_valid;
    auto alive = mem.isProcessAlive();

    if (previous && !alive)
    {
        std::cout << "Connection to CS2 lost." << std::endl;
        return false;
    }

    if (!alive)
    {
        alive = mem.connectToProcess(L"cs2.exe");

        if (alive)
            std::cout << "Connection to CS2 established." << std::endl;
    }

    isConnected = alive;

    return alive;
}

void run_cheat_loop(bool &terminate)
{
    Memory mem;
    auto entity_list = new EntityData();
    auto cheat_data = new CheatData();

    auto lastTime = std::chrono::high_resolution_clock::now();

    for (size_t iteration = 0; !terminate; ++iteration)
    {
        if (!check_connection(mem))
        {
            std::this_thread::sleep_for(delays::memConnectionWaitTime);
            lastTime = std::chrono::high_resolution_clock::now();
            continue;
        }

        lastTime += std::chrono::milliseconds(1);

        std::this_thread::sleep_until(lastTime);

        if (!read_entity_data(mem, *entity_list) || !read_cheat_data(mem, *entity_list, *cheat_data))
        {
            espData.isValid = false;
            continue;
        }

        run_all_cheats(mem, *cheat_data, iteration);
    }

    delete cheat_data;
    delete entity_list;

    std::cout << "Cheat thread terminated." << std::endl;
}
