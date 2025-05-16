#include "../strings.h"

#include <Windows.h>

void cleanString(char* input, char* buffer, size_t bufferSize) {
	size_t buffer_idx = 0;
	char sym = '\0';
	for (size_t in_idx = 0; (sym = input[in_idx]) != '\0' && buffer_idx < (bufferSize - 1); ++in_idx)
	{
		if (32 <= sym && sym <= 126)
			buffer[buffer_idx++] = sym;
	}

	while (buffer_idx < bufferSize)
		buffer[buffer_idx++] = 0;

	buffer[buffer_idx] = '\0';
}

static std::wstring char_to_wstring(const char* str, size_t size) {
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, str, (int)size, NULL, 0);

	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, str, (int)size, &wstrTo[0], size_needed);

	return wstrTo;
}

std::wstring string_to_wstring(const std::string& str) {
	return char_to_wstring(str.c_str(), str.size());
}

std::wstring string_to_wstring(const std::string_view& str) {
	return char_to_wstring(str.data(), str.size());
}
