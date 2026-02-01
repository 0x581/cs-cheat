#pragma once

#include "json.h"

#include "keyboard.hpp"

class ToggleableKeybind
{
public:
    int key;
    bool isToggle = false;

private:
    bool toggleState = false;
    bool pressed = false;
    bool waitingForKeyRelease = false;
    bool editing = false;

public:
    ToggleableKeybind(int key = -1, bool isToggle = false) : key(key), isToggle(isToggle) {}

    void setEditing(bool editing);

    // Updates the Keybind state. Should only be called once per iteration. Use .get() after.
    void update();

    // Checks the current state of the keybind and returns whether it represents the state "enabled".
    bool get() const;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ToggleableKeybind, key, isToggle, toggleState)
};

class Keybind
{
public:
    int key;

private:
    bool pressed = false;
    bool waitingForKeyRelease = false;
    bool editing = false;

public:
    Keybind(int key = -1) : key(key) {}

    void awaitKeyRelease()
    {
        this->waitingForKeyRelease = true;
    }

    void setEditing(bool editing);

    // Updates the Keybind state. Should only be called once per iteration. Use .get() after.
    void update();

    // Checks the current state of the keybind and returns whether it represents the state "enabled".
    bool get() const;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Keybind, key)
};
