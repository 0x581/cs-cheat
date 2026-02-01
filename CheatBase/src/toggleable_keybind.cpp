#include "../toggleable_keybind.hpp"

#include "keyboard.hpp"

void ToggleableKeybind::setEditing(bool editing)
{
    this->editing = editing;

    if (editing)
    {
        this->waitingForKeyRelease = false;
        this->pressed = false;
    }
}

void ToggleableKeybind::update()
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

bool ToggleableKeybind::get() const
{
    if (this->editing)
        return false;

    if (this->isToggle)
        return this->toggleState;

    if (this->waitingForKeyRelease)
        return false;

    return this->pressed;
}
