#include "map_driver.h"

#include <Windows.h>

#include <filesystem>

#include <files.h>
#include <strings.h>
#include <iostream>

namespace fs = std::filesystem;
using fs::path;

static bool get_expanded_path(const std::string_view folder, path& expandedPath) {
	const auto wstring_folder = string_to_wstring(folder);

	const auto required_size = ::ExpandEnvironmentStringsW(wstring_folder.c_str(), nullptr, 0);

	if (required_size == 0)
		return false;

	wchar_t* folder_expanded = new wchar_t[required_size];
	const auto parsed_length = ::ExpandEnvironmentStringsW(wstring_folder.c_str(), folder_expanded, required_size);

	if (parsed_length != required_size)
	{
		delete[] folder_expanded;
		return false;
	}

	expandedPath = fs::absolute(folder_expanded);
	delete[] folder_expanded;

	return true;
}


bool try_copy_to_folder(const path src_folder, const path dst_folder, const std::string_view& filename) {
	if (!fs::exists(src_folder / filename))
		return false;

	if (!fs::exists(dst_folder))
		fs::create_directories(dst_folder);

	const auto src = src_folder / filename;
	const auto dst = dst_folder / filename;

	return fs::copy_file(src, dst, fs::copy_options::overwrite_existing);
}


void map_driver() {
	std::cout << "Trying to map driver..." << std::endl;

	const auto cur_dir = fs::current_path();

	path out_dir;
	if (!get_expanded_path(files::folder, out_dir))
	{
		std::cout << "Failed to get expanded path for folder: " << files::folder << std::endl;
		return;
	}

	try_copy_to_folder(cur_dir, out_dir, driver_files::driverName);
	try_copy_to_folder(cur_dir, out_dir, driver_files::kdmapper);

	const auto kdmapper = out_dir / driver_files::kdmapper;
	const auto driver = out_dir / driver_files::driverName;

	if (!fs::exists(driver) || !fs::exists(kdmapper))
	{
		std::cout << "Failed to find kdmapper or driver." << std::endl;
		return;
	}

	std::wstring kdmapper_ws = kdmapper.wstring();
	std::wstring driver_ws = L"\"" + driver.wstring() + L"\""; // Quote if path might have spaces

	SHELLEXECUTEINFOW sei = { sizeof(sei) };
	sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
	sei.hwnd = NULL;
	sei.lpVerb = L"runas";
	sei.lpFile = kdmapper_ws.c_str();
	sei.lpParameters = driver_ws.c_str();
	sei.lpDirectory = out_dir.wstring().c_str();
	sei.nShow = SW_SHOWDEFAULT;

	std::wcout << "Executing command: " << kdmapper_ws << L" " << driver_ws << std::endl;

	auto success = ::ShellExecuteExW(&sei);

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
