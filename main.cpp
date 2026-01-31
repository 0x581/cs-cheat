
#include <iostream>
#include <mutex>
#include <thread>

#include "CheatBase/keyboard.hpp"

#include "files.hpp"
#include "config.hpp"

#include "overlay/overlay.hpp"

#include "cheats/cheats.hpp"

static c_overlay *overlay;

std::mutex mtx;
std::condition_variable cv;
bool done_flag = false;

static void cleanUp()
{
    delete overlay;

    cfg.store_persistent_config();
}

static BOOL WINAPI ConsoleCtrlHandler(DWORD fdwCtrlType)
{
    switch (fdwCtrlType)
    {
    case CTRL_BREAK_EVENT:
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
        if (overlay)
            overlay->exit = true;

        std::unique_lock<std::mutex> lock(mtx);

        if (done_flag)
            return TRUE;

        cv.wait(lock, []
                { return done_flag; });

        return TRUE;
    }
    return FALSE;
}

int main()
{
    if (!SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE))
    {
        std::cerr << "Error: Failed to set console control handler." << std::endl;
        return 1;
    }

    cfg.verbose = true;

    cfg.set_working_directory(files::config_folder);
    cfg.load_all_config_data();

    overlay = new c_overlay();

    bool terminate = false;

#ifndef DISABLE_INPUT_HOOK
    std::thread input_hook{run_input_hook, std::ref(terminate)};
#else
    std::cout << "Input hook is disabled." << std::endl;
#endif

    std::thread cheats_thread{run_cheat_loop, std::ref(terminate)};

    overlay->run();
    terminate = true;

#ifndef DISABLE_INPUT_HOOK
    input_hook.join();
#endif

    cheats_thread.join();

    cleanUp();

    std::lock_guard<std::mutex> lock(mtx);
    done_flag = true;

    cv.notify_all();
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    return main();
}
