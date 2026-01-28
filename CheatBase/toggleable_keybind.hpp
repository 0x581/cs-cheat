#pragma once

#include "json.h"

struct ToggleableKeybind
{
    int key;
    bool isToggle = false;
    bool toggleState = false;
    bool keybindChanged = false;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ToggleableKeybind, key, isToggle, toggleState, keybindChanged)
