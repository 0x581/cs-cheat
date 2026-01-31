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

    void setEditing(bool editing)
    {
        this->editing = editing;

        if (editing)
        {
            this->waitingForKeyRelease = false;
            this->pressed = false;
        }
    }

    // Updates the Keybind state. Should only be called once per iteration. Use .get() after.
    void update()
    {
        if (this->editing)
            return;

        const bool isKeyPressed = IsKeyPressed(this->key);

        if (this->isToggle)
        {
            if (!this->waitingForKeyRelease && isKeyPressed)
                this->toggleState = !this->toggleState;

            this->waitingForKeyRelease = isKeyPressed;
            return;
        }
        else
        {
            if (this->waitingForKeyRelease && isKeyPressed)
                return;

            this->waitingForKeyRelease = false;

            this->pressed = isKeyPressed;
        }
    }

    // Checks the current state of the keybind and returns whether it represents the state "enabled".
    bool get() const
    {
        if (this->editing)
            return false;

        if (this->isToggle)
            return this->toggleState;

        if (this->waitingForKeyRelease)
            return false;

        return this->pressed;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ToggleableKeybind, key, isToggle, toggleState)
};
