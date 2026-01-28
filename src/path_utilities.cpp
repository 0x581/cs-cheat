#include "path_utilities.hpp"

#include <Windows.h>

#include "string_utilities.hpp"

bool get_expanded_path(const std::wstring_view folder, std::filesystem::path &expandedPath)
{
    namespace fs = std::filesystem;

    const auto required_size = ExpandEnvironmentStrings(folder.data(), nullptr, 0);

    if (required_size == 0)
        return false;

    wchar_t *folder_expanded = new wchar_t[required_size];
    const auto parsed_length = ExpandEnvironmentStrings(folder.data(), folder_expanded, required_size);

    if (parsed_length != required_size)
    {
        delete[] folder_expanded;
        return false;
    }

    expandedPath = fs::absolute(folder_expanded);
    delete[] folder_expanded;

    return true;
}

bool get_expanded_path(const std::string_view folder, std::filesystem::path &expandedPath)
{
    const std::wstring wstring_folder = string_to_wstring(folder);
    return get_expanded_path(wstring_folder, expandedPath);
}

bool copy_file_into_folder(const std::filesystem::path file, const std::filesystem::path dst_folder)
{
    namespace fs = std::filesystem;

    if (!fs::exists(file))
        return false;

    if (!fs::exists(dst_folder) || !fs::create_directories(dst_folder))
        return false;

    const auto dst_file = dst_folder / file.filename();

    const auto copy_options = std::filesystem::copy_options::overwrite_existing;

    return fs::copy_file(file, dst_file, copy_options);
}
