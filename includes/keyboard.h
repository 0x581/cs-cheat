#pragma once

extern int mostRecentKey;

const bool IsKeyPressed(const int vkCode);

bool init_input_hook();

void clear_input_hook();
