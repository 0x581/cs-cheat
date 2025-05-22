#pragma once

extern bool g_keyStates[256];

bool IsKeyPressed(int vkCode);

bool init_keyboard_hook();