#include "../../overlay.h"

#include <config.h>
#include <shared_variables.h>


void c_overlay::draw_bomb()
{
	ImGui::SetNextWindowPos(persistentConfig.bombWindow.position, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(bombSize);

	ImGui::Begin(
		"Bomb GUI",
		0,
		ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoBackground
	);

	persistentConfig.bombWindow.position = ImGui::GetWindowPos();

	ImGui::PushFont(bombFont1);

	if (bombData.planted == true)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
		ImGui::Text("Bomb has been planted.");
		if (bombData.timeRemaining > 0)
		{
			ImGui::Text("Time remaining: %.2f", bombData.timeRemaining);
		}
		else
		{
			ImGui::Text("If I was you i would RUN!!");
		}
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
		ImGui::Text("Bomb is not planted.");
	}
	ImGui::PopStyleColor();

	ImGui::PopFont();
	ImGui::End();
}
