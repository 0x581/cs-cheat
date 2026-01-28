#include "memory.hpp"

#include "files.hpp"
#include "path_utilities.hpp"

#include <Windows.h>

#include <filesystem>

#include <iostream>

namespace fs = std::filesystem;
using fs::path;

void MapDriver::map_driver()
{
    std::cout << "Trying to map driver..." << std::endl;

    const auto cur_dir = fs::current_path();

    path out_dir;
    if (!get_expanded_path(driver_files::folder, out_dir))
    {
        std::cout << "Failed to get expanded path for folder: " << driver_files::folder << std::endl;
        return;
    }

    copy_file_into_folder(cur_dir / driver_files::driverName, out_dir);
    copy_file_into_folder(cur_dir / driver_files::kdmapper, out_dir);

    const auto kdmapper = out_dir / driver_files::kdmapper;
    const auto driver = out_dir / driver_files::driverName;

    if (!fs::exists(driver) || !fs::exists(kdmapper))
    {
        std::cout << "Failed to find kdmapper or driver." << std::endl;
        return;
    }

    std::wstring kdmapper_ws = kdmapper.wstring();
    std::wstring driver_ws = L"\"" + driver.wstring() + L"\""; // Quote if path might have spaces

    SHELLEXECUTEINFO sei = {sizeof(sei)};
    sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
    sei.hwnd = NULL;
    sei.lpVerb = L"runas";
    sei.lpFile = kdmapper_ws.c_str();
    sei.lpParameters = driver_ws.c_str();
    sei.lpDirectory = out_dir.wstring().c_str();
    sei.nShow = SW_SHOWDEFAULT;

    std::wcout << "Executing command: " << kdmapper_ws << L" " << driver_ws << std::endl;

    auto success = ShellExecuteEx(&sei);

    success &= sei.hProcess != NULL;

    if (success)
    {
        WaitForSingleObject(sei.hProcess, INFINITE);

        DWORD exitCode = 0;
        if (GetExitCodeProcess(sei.hProcess, &exitCode))
            success = (exitCode == 0);
        else
            success = false;
        CloseHandle(sei.hProcess);
    }

    if (success)
        std::cout << "Driver mapped successfully." << std::endl;
    else
        std::cout << "Failed to execute kdmapper." << std::endl;
}
