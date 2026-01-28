#pragma once

#include <string>

void cleanString(char *input, char *buffer, size_t bufferSize);

std::wstring string_to_wstring(const std::string &str);
std::wstring string_to_wstring(const std::string_view &str);

std::string wstring_to_string(const std::wstring &str);
std::string wstring_to_string(const std::wstring_view &str);
