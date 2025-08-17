
#include <thread>
#include <iostream>

#include <mutex>

#include <files.h>
#include <config.h>

#include "../overlay/overlay.h"

#include "cheats/cheats.h"
#include <keyboard.h>

static c_overlay *overlay;

std::mutex mtx;
std::condition_variable cv;
bool done_flag = false;

static void cleanUp()
{
    delete overlay;

    store_persistent_config(files::folder);
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

    load_all_config_data(files::folder);

    overlay = new c_overlay();

    bool terminate = false;
    std::thread input_hook{run_input_hook, std::ref(terminate)};
    std::thread cheats_thread{run_cheat_loop, std::ref(terminate)};

    overlay->run();
    terminate = true;

    input_hook.join();
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
