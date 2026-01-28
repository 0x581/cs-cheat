#pragma once

#include "toggleable_keybind.hpp"

extern bool editing_hotkey;

bool HotkeyEdit(const char *label, int *key_value);

bool HotkeyEdit(const char *label, ToggleableKeybind &keybind);
