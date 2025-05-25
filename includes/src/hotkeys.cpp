
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

#include <string>
#include <keyboard.h>
#include <strings.h>

bool editing_hotkey = false;


static const char* get_key_name(const int key)
{
#define NAME_CASE(id, key_name) case id: return key_name;

	switch (key)
	{
		NAME_CASE(-1, "None");
		NAME_CASE(VK_LBUTTON, "LMB");
		NAME_CASE(VK_RBUTTON, "RMB");
		NAME_CASE(VK_MBUTTON, "MMB");
		NAME_CASE(VK_XBUTTON1, "MB 4");
		NAME_CASE(VK_XBUTTON2, "MB 5");
		NAME_CASE(VK_BACK, "Backspace");
		NAME_CASE(VK_TAB, "Tab");
		NAME_CASE(VK_CLEAR, "Clear");
		NAME_CASE(VK_RETURN, "Enter");
		NAME_CASE(VK_SHIFT, "Shift");
		NAME_CASE(VK_CONTROL, "Ctrl");
		NAME_CASE(VK_MENU, "Alt");
		NAME_CASE(VK_PAUSE, "Pause");
		NAME_CASE(VK_CAPITAL, "Caps");

		NAME_CASE(VK_SPACE, "Space");
		NAME_CASE(VK_PRIOR, "Page Up");
		NAME_CASE(VK_NEXT, "Page Dn");
		NAME_CASE(VK_END, "End");
		NAME_CASE(VK_HOME, "Home");
		NAME_CASE(VK_LEFT, "Left");
		NAME_CASE(VK_UP, "Up");
		NAME_CASE(VK_RIGHT, "Right");
		NAME_CASE(VK_DOWN, "Down");
		NAME_CASE(VK_SELECT, "Select");
		NAME_CASE(VK_PRINT, "Print");
		NAME_CASE(VK_EXECUTE, "Exec");
		NAME_CASE(VK_SNAPSHOT, "Print Screen");
		NAME_CASE(VK_INSERT, "Ins");
		NAME_CASE(VK_DELETE, "Del");
		NAME_CASE(VK_HELP, "Help");

		NAME_CASE('0', "0");
		NAME_CASE('1', "1");
		NAME_CASE('2', "2");
		NAME_CASE('3', "3");
		NAME_CASE('4', "4");
		NAME_CASE('5', "5");
		NAME_CASE('6', "6");
		NAME_CASE('7', "7");
		NAME_CASE('8', "8");
		NAME_CASE('9', "9");

		NAME_CASE('A', "A");
		NAME_CASE('B', "B");
		NAME_CASE('C', "C");
		NAME_CASE('D', "D");
		NAME_CASE('E', "E");
		NAME_CASE('F', "F");
		NAME_CASE('G', "G");
		NAME_CASE('H', "H");
		NAME_CASE('I', "I");
		NAME_CASE('J', "J");
		NAME_CASE('K', "K");
		NAME_CASE('L', "L");
		NAME_CASE('M', "M");
		NAME_CASE('N', "N");
		NAME_CASE('O', "O");
		NAME_CASE('P', "P");
		NAME_CASE('Q', "Q");
		NAME_CASE('R', "R");
		NAME_CASE('S', "S");
		NAME_CASE('T', "T");
		NAME_CASE('U', "U");
		NAME_CASE('V', "V");
		NAME_CASE('W', "W");
		NAME_CASE('X', "X");
		NAME_CASE('Y', "Y");
		NAME_CASE('Z', "Z");

		NAME_CASE(VK_NUMPAD0, "Num 0");
		NAME_CASE(VK_NUMPAD1, "Num 1");
		NAME_CASE(VK_NUMPAD2, "Num 2");
		NAME_CASE(VK_NUMPAD3, "Num 3");
		NAME_CASE(VK_NUMPAD4, "Num 4");
		NAME_CASE(VK_NUMPAD5, "Num 5");
		NAME_CASE(VK_NUMPAD6, "Num 6");
		NAME_CASE(VK_NUMPAD7, "Num 7");
		NAME_CASE(VK_NUMPAD8, "Num 8");
		NAME_CASE(VK_NUMPAD9, "Num 9");

		NAME_CASE(VK_MULTIPLY, "Num *");
		NAME_CASE(VK_ADD, "Num +");
		NAME_CASE(VK_SEPARATOR, "Num Enter");
		NAME_CASE(VK_SUBTRACT, "Num -");
		NAME_CASE(VK_DECIMAL, "Num .");
		NAME_CASE(VK_DIVIDE, "Num /");

		NAME_CASE(VK_F1, "F1");
		NAME_CASE(VK_F2, "F2");
		NAME_CASE(VK_F3, "F3");
		NAME_CASE(VK_F4, "F4");
		NAME_CASE(VK_F5, "F5");
		NAME_CASE(VK_F6, "F6");
		NAME_CASE(VK_F7, "F7");
		NAME_CASE(VK_F8, "F8");
		NAME_CASE(VK_F9, "F9");
		NAME_CASE(VK_F10, "F10");
		NAME_CASE(VK_F11, "F11");
		NAME_CASE(VK_F12, "F12");
		NAME_CASE(VK_F13, "F13");
		NAME_CASE(VK_F14, "F14");
		NAME_CASE(VK_F15, "F15");
		NAME_CASE(VK_F16, "F16");
		NAME_CASE(VK_F17, "F17");
		NAME_CASE(VK_F18, "F18");
		NAME_CASE(VK_F19, "F19");
		NAME_CASE(VK_F20, "F20");
		NAME_CASE(VK_F21, "F21");
		NAME_CASE(VK_F22, "F22");
		NAME_CASE(VK_F23, "F23");
		NAME_CASE(VK_F24, "F24");

		NAME_CASE(VK_NUMLOCK, "Num Lk");
		NAME_CASE(VK_SCROLL, "Scrl Lk");

		NAME_CASE(VK_LSHIFT, "L Shift");
		NAME_CASE(VK_RSHIFT, "R Shift");
		NAME_CASE(VK_LCONTROL, "L Ctrl");
		NAME_CASE(VK_RCONTROL, "R Ctrl");
		NAME_CASE(VK_LMENU, "L Alt");
		NAME_CASE(VK_RMENU, "R Alt");
	}

	return nullptr;
}


static const std::string get_localized_key_name(const int vkCode)
{
	const char* keyName = get_key_name(vkCode);

	if (keyName != nullptr)
		return keyName;

	const auto scanCode = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC);

	LONG lParam = scanCode << 16;

	if (vkCode == VK_RCONTROL || vkCode == VK_RMENU)
	{
		lParam |= (1 << 24); // Set extended key flag
	}

	wchar_t keyNameBuffer[256];
	int result = GetKeyNameText(lParam, keyNameBuffer, sizeof(keyNameBuffer) / sizeof(keyNameBuffer[0]));

	if (result > 0)
	{
		return wstring_to_string(std::wstring_view(keyNameBuffer));
	}

	return "INVALID";
}



bool HotkeyEdit(const char* label, int* key_value)
{
	const ImGuiStyle& style = ImGui::GetStyle();

	int* last_pressed_key = &mostRecentKey;

	bool value_changed = false;

	// Create a unique ID for this widget
	ImGui::PushID(label);

	const auto key_text_str = get_localized_key_name(*key_value);



	// Calculate button size (square)
	const float height = ImGui::GetFrameHeight();
	const float width = style.FramePadding.x * 2 + ImGui::CalcTextSize(key_text_str.c_str()).x;

	// Draw the button with the key
	if (ImGui::Button(key_text_str.c_str(), ImVec2(width, height)))
	{
		// Clear the last pressed key and open popup
		*last_pressed_key = -1;

		ImGui::OpenPopup("hotkey_popup");
	}

	// Same line for label
	ImGui::SameLine();
	ImGui::Text("%s", label);

	// Hotkey selection popup
	if (ImGui::BeginPopup("hotkey_popup"))
	{
		editing_hotkey = true;

		ImGui::Text("Press a key...");
		ImGui::Text("(ESC to cancel)");

		// Check if a key was pressed
		if (*last_pressed_key != -1)
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

		ImGui::EndPopup();
	}

	ImGui::PopID();
	return value_changed;
}
