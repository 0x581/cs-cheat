#pragma once

#include <filesystem>
#include <string_view>

bool get_expanded_path(const std::wstring_view folder, std::filesystem::path &expandedPath);

bool get_expanded_path(const std::string_view folder, std::filesystem::path &expandedPath);

bool copy_file_into_folder(const std::filesystem::path file, const std::filesystem::path dst_folder);
