#include "../strings.h"

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
