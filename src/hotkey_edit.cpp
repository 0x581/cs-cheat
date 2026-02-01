#include "../hotkey_edit.hpp"

#include <string>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

#include "keyboard.hpp"
#include "string_utilities.hpp"

bool editing_hotkey = false;

static std::array<const char *, 166> key_name_list = {
    0,              // 0x00
    "LMB",          // 0x01
    "RMB",          // 0x02
    0,              // 0x03
    "MMB",          // 0x04
    "MB 4",         // 0x05
    "MB 5",         // 0x06
    0,              // 0x07
    "Backspace",    // 0x08
    "Tab",          // 0x09
    0,              // 0x0A
    0,              // 0x0B
    "Clear",        // 0x0C
    "Enter",        // 0x0D
    0,              // 0x0E
    0,              // 0x0F
    "Shift",        // 0x10
    "Ctrl",         // 0x11
    "Alt",          // 0x12
    "Pause",        // 0x13
    "Caps Lock",    // 0x14
    0,              // 0x15
    0,              // 0x16
    0,              // 0x17
    0,              // 0x18
    0,              // 0x19
    0,              // 0x1A
    "Esc",          // 0x1B
    0,              // 0x1C
    0,              // 0x1D
    0,              // 0x1E
    0,              // 0x1F
    "Space",        // 0x20
    "Page Up",      // 0x21
    "Page Down",    // 0x22
    "End",          // 0x23
    "Home",         // 0x24
    "Left",         // 0x25
    "Up",           // 0x26
    "Right",        // 0x27
    "Down",         // 0x28
    "Select",       // 0x29
    "Print",        // 0x2A
    "Exec",         // 0x2B
    "Print Screen", // 0x2C
    "Insert",       // 0x2D
    "Delete",       // 0x2E
    "Help",         // 0x2F
    "0",            // 0x30
    "1",            // 0x31
    "2",            // 0x32
    "3",            // 0x33
    "4",            // 0x34
    "5",            // 0x35
    "6",            // 0x36
    "7",            // 0x37
    "8",            // 0x38
    "9",            // 0x39
    0,              // 0x3A
    0,              // 0x3B
    0,              // 0x3C
    0,              // 0x3D
    0,              // 0x3E
    0,              // 0x3F
    0,              // 0x40
    "A",            // 0x41
    "B",            // 0x42
    "C",            // 0x43
    "D",            // 0x44
    "E",            // 0x45
    "F",            // 0x46
    "G",            // 0x47
    "H",            // 0x48
    "I",            // 0x49
    "J",            // 0x4A
    "K",            // 0x4B
    "L",            // 0x4C
    "M",            // 0x4D
    "N",            // 0x4E
    "O",            // 0x4F
    "P",            // 0x50
    "Q",            // 0x51
    "R",            // 0x52
    "S",            // 0x53
    "T",            // 0x54
    "U",            // 0x55
    "V",            // 0x56
    "W",            // 0x57
    "X",            // 0x58
    "Y",            // 0x59
    "Z",            // 0x5A
    0,              // 0x5B
    0,              // 0x5C
    0,              // 0x5D
    0,              // 0x5E
    0,              // 0x5F
    "Num 0",        // 0x60
    "Num 1",        // 0x61
    "Num 2",        // 0x62
    "Num 3",        // 0x63
    "Num 4",        // 0x64
    "Num 5",        // 0x65
    "Num 6",        // 0x66
    "Num 7",        // 0x67
    "Num 8",        // 0x68
    "Num 9",        // 0x69
    "Num *",        // 0x6A
    "Num +",        // 0x6B
    "Num Enter",    // 0x6C
    "Num -",        // 0x6D
    "Num .",        // 0x6E
    "Num /",        // 0x6F
    "F1",           // 0x70
    "F2",           // 0x71
    "F3",           // 0x72
    "F4",           // 0x73
    "F5",           // 0x74
    "F6",           // 0x75
    "F7",           // 0x76
    "F8",           // 0x77
    "F9",           // 0x78
    "F10",          // 0x79
    "F11",          // 0x7A
    "F12",          // 0x7B
    "F13",          // 0x7C
    "F14",          // 0x7D
    "F15",          // 0x7E
    "F16",          // 0x7F
    "F17",          // 0x80
    "F18",          // 0x81
    "F19",          // 0x82
    "F20",          // 0x83
    "F21",          // 0x84
    "F22",          // 0x85
    "F23",          // 0x86
    "F24",          // 0x87
    0,              // 0x88
    0,              // 0x89
    0,              // 0x8A
    0,              // 0x8B
    0,              // 0x8C
    0,              // 0x8D
    0,              // 0x8E
    0,              // 0x8F
    "Num Lk",       // 0x90
    "Scrl Lk",      // 0x91
    0,              // 0x92
    0,              // 0x93
    0,              // 0x94
    0,              // 0x95
    0,              // 0x96
    0,              // 0x97
    0,              // 0x98
    0,              // 0x99
    0,              // 0x9A
    0,              // 0x9B
    0,              // 0x9C
    0,              // 0x9D
    0,              // 0x9E
    0,              // 0x9F
    "L Shift",      // 0xA0
    "R Shift",      // 0xA1
    "L Ctrl",       // 0xA2
    "R Ctrl",       // 0xA3
    "L Alt",        // 0xA4
    "R Alt",        // 0xA5
};

static const char *get_key_name(const int key)
{
    if (key == -1)
        return "None";

    if (key >= 0 && key < static_cast<int>(key_name_list.size()))
    {
        const char *name = key_name_list[key];

        if (name != nullptr)
            return name;
    }

    return nullptr;
}

static const std::string get_localized_key_name(const int vkCode)
{
    const char *keyName = get_key_name(vkCode);

    if (keyName != nullptr)
        return keyName;

    const auto scanCode = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC);

    LONG lParam = scanCode << 16;

    if (vkCode == VK_RCONTROL || vkCode == VK_RMENU)
    {
        lParam |= (1 << 24); // Set extended key flag
    }

    constexpr auto keyNameBufferSize = 256;

    char keyNameBuffer[keyNameBufferSize];
    int result = GetKeyNameTextA(lParam, keyNameBuffer, keyNameBufferSize);

    if (result > 0)
        return std::string(keyNameBuffer, result);

    return "INVALID";
}

static bool HotkeyEdit(const char *label, int *key_value, bool *is_toggle, bool *editing)
{
    static int last_frame_value = -1;

    const ImGuiStyle &style = ImGui::GetStyle();

    int *last_pressed_key = &mostRecentKey;

    bool value_changed = false;
    bool toggle_changed = false;

    // Create a unique ID for this widget
    ImGui::PushID(label);

    const auto key_text_str = get_localized_key_name(*key_value);

    // Calculate toggle button width
    const float toggle_width = is_toggle != nullptr ? style.FramePadding.x * 2 + ImGui::CalcTextSize("Toggle").x : 0;

    // Calculate button size (square)
    const float width = style.FramePadding.x * 2 + ImGui::CalcTextSize(key_text_str.c_str()).x;

    ImGui::AlignTextToFramePadding();
    ImGui::Text(label);
    ImGui::SameLine(style.WindowPadding.x + ImGui::GetContentRegionAvail().x - width - (toggle_width ? toggle_width + style.ItemSpacing.x : 0));

    // Draw the button with the key
    if (ImGui::Button(key_text_str.c_str()))
    {
        // Clear the last pressed key and open popup
        *last_pressed_key = -1;
        last_frame_value = -1;

        ImGui::OpenPopup("hotkey_popup");
    }

    // Hotkey selection popup
    if (ImGui::BeginPopup("hotkey_popup"))
    {
        if (editing)
            *editing = true;

        editing_hotkey = true;

        ImGui::Text("Press a key...");
        ImGui::Text("(ESC to cancel)");

        // Check if a key was pressed
        if (*last_pressed_key != -1 && last_frame_value == *last_pressed_key)
        {
            if (*last_pressed_key == 27)
            { // ESC key cancels
                ImGui::CloseCurrentPopup();
            }
            else
            {
                *key_value = *last_pressed_key;
                value_changed = true;
                ImGui::CloseCurrentPopup();
            }
        }

        last_frame_value = *last_pressed_key;

        ImGui::EndPopup();
    }
    else if (editing)
        *editing = false;

    // Draw the toggle button if is_toggle is not null
    if (is_toggle != nullptr)
    {
        ImGui::SameLine();

        if (ImGui::Button(*is_toggle ? "Toggle" : "Hold", ImVec2(toggle_width, 0)))
            toggle_changed = true;

        if (toggle_changed)
            *is_toggle = !*is_toggle;
    }

    ImGui::PopID();

    return value_changed || toggle_changed;
}

bool HotkeyEdit(const char *label, int *key_value)
{
    return HotkeyEdit(label, key_value, nullptr, nullptr);
}

bool HotkeyEdit(const char *label, ToggleableKeybind &keybind)
{
    bool edited = false;
    const auto ret = HotkeyEdit(label, &keybind.key, &keybind.isToggle, &edited);
    keybind.setEditing(edited);
    return ret;
}

bool HotkeyEdit(const char *label, Keybind &keybind)
{
    bool edited = false;
    const auto ret = HotkeyEdit(label, &keybind.key, nullptr, &edited);
    keybind.setEditing(edited);
    return ret;
}
