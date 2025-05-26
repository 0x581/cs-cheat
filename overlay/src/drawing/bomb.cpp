#include "../../overlay.h"

#include <config.h>
#include <shared_variables.h>


void c_overlay::draw_bomb()
{
	ImGui::SetNextWindowPos(persistentConfig.bombWindow.position, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(bombScale);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin(
		"Bomb GUI",
		0,
		ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoScrollWithMouse
		| (show_menu ? 0 : ImGuiWindowFlags_NoBackground)
	);

	persistentConfig.bombWindow.position = ImGui::GetWindowPos();

	ImGui::PushFont(bombFont1);

	std::vector<std::string> lines;

	if (!bombData.planted)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
		lines.push_back(std::string("Bomb is not planted."));
	}
	if (bombData.planted && bombData.timeRemaining < 0.f)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
		lines.push_back(std::string("Bum"));
	}
	if (bombData.planted && bombData.defused)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
		lines.push_back(std::string("Bomb has been defused."));
	}
	if (bombData.planted && !bombData.defused)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
		lines.push_back(std::string("Bomb has been planted on ") + (bombData.site ? "B.\n" : "A.\n"));

		const auto time_str = std::to_string(bombData.timeRemaining);

		lines.push_back(time_str.substr(0, time_str.find('.') + 3) + " seconds remaining.");
	}

	const auto available = ImGui::GetContentRegionAvail();

	const auto font_size = ImGui::GetFontSize();

	const auto total_height = font_size * lines.size();

	float current_height = 0.f;

	const auto start_cursor_pos = ImGui::GetCursorPos();

	for (const auto& line : lines)
	{
		const auto line_size = ImGui::CalcTextSize(line.c_str());

		const auto text_x = (available.x - line_size.x) * 0.5f;
		const auto text_y = (available.y - total_height) * 0.5f + current_height;

		ImGui::SetCursorPos(start_cursor_pos + ImVec2(text_x, text_y));

		ImGui::TextUnformatted(line.c_str());

		current_height += font_size;
	}



	// line1_text += std::string("\nLocal Player Can Defuse: ") + (bombData.localPlayerCanDefuse ? "Yes" : "No");
	// line1_text += std::string("\nBeing Defused: ") + (bombData.beingDefused ? "Yes" : "No");
	// line1_text += std::string("\nDefuser: ") + (bombData.defuser ? std::to_string(bombData.defuser) : "None");


	ImGui::PopStyleColor();

	ImGui::PopFont();
	ImGui::End();

	ImGui::PopStyleVar();
}
