#include "../../overlay.h"

#include <config.h>
#include <files.h>


static int current_main_nav_item = 0;
const char* nav_items[] = { "Aim", "Visuals", "Misc", "Config" };


static char configNameBuffer[CONFIG_NAME_MAX_LENGTH] = "";

enum MenuOrder {
	AIM,
	VISUALS,
	MISC,
	CONFIG
};

void c_overlay::draw_menu() {
	ImGui::SetCurrentContext(menuContext);


	ImGuiIO& io = ImGui::GetIO();
	if (ImGui::IsItemActive())
	{
		ImGui::Text("Widget is active: %s", io.WantTextInput ? "Capturing text" : "NOT capturing text");
		ImGui::Text("WantCaptureKeyboard: %s", io.WantCaptureKeyboard ? "Yes" : "No");
	}


	ImGuiStyle& style = ImGui::GetStyle();

	ImGui::SetNextWindowPos(persistentConfig.mainWindow.windowPosition, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(menu_width, menu_height), ImGuiCond_FirstUseEver);

	ImGui::Begin("Main GUI",
		nullptr,
		ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoSavedSettings
	);

	persistentConfig.mainWindow.windowPosition = ImGui::GetWindowPos();

	float nav_bar_width = 130.0f;
	ImGui::BeginChild("NavBar", ImVec2(nav_bar_width, 0), true, ImGuiWindowFlags_NoScrollbar);
	{
		ImGui::PushFont(font->im_font);
		ImGui::Text("Merkur");
		ImGui::PopFont();
		ImGui::Separator();
		ImGui::Spacing();
		for (int i = 0; i < IM_ARRAYSIZE(nav_items); ++i)
		{
			bool is_selected = (current_main_nav_item == i);
			if (is_selected)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_TabActive]);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, style.Colors[ImGuiCol_TabActive]);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, style.Colors[ImGuiCol_TabActive]);
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_Tab]);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, style.Colors[ImGuiCol_TabHovered]);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, style.Colors[ImGuiCol_TabActive]);
			}
			if (ImGui::Button(nav_items[i], ImVec2(nav_bar_width - style.WindowPadding.x * 2, 30)))
			{
				current_main_nav_item = i;
			}
			ImGui::PopStyleColor(3);
			ImGui::Spacing();
		}
	}
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("ContentArea", ImVec2(0, 0), true);
	{
		ImGui::Text("%s Settings", nav_items[current_main_nav_item]);
		ImGui::Separator();
		ImGui::Spacing();

		float label_width = 80.0f;

		switch (current_main_nav_item)
		{
		case MenuOrder::AIM:
			if (ImGui::CollapsingHeader("Aimbot Options", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Checkbox("Triggerbot", &config.activeCheats.triggerBot);
				ImGui::Checkbox("Enable Aimbot", &config.activeCheats.aimBot);
				if (config.activeCheats.aimBot)
				{
					if (ImGui::CollapsingHeader("Ragebot Options", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::Checkbox("Enable Ragebot", &config.activeCheats.rageBot);
						if (config.activeCheats.rageBot)
						{

							ImGui::Text("Ragebot use the aimbot funktions.");
							ImGui::Text("Shot Delay ");
							ImGui::SameLine(label_width + style.ItemInnerSpacing.x);
							ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - style.ItemInnerSpacing.x);
							ImGui::SliderInt("##RageShotDelay", &config.rageBot.shotDelay, 0, 200, "%d ms");
							ImGui::PopItemWidth();

						}
					}
					ImGui::Indent();

					ImGui::Checkbox("Draw FOV Circle", &config.aimBot.drawFovCircle);
					if (config.aimBot.drawFovCircle)
					{
						ImGui::SameLine(0, style.ItemSpacing.x * 2);
						ImGui::ColorEdit4("##FovCircleColor", (float*)&config.aimBot.fovCircleColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
						ImGui::Text("FOV");
						ImGui::SameLine(label_width + style.ItemInnerSpacing.x);
						ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - style.ItemInnerSpacing.x);
						ImGui::SliderFloat("##FOV", &config.aimBot.pixelRadius, 0.0f, 400.0f, "%.0fpx");
						ImGui::PopItemWidth();
					}
					ImGui::Text("Smooth");
					ImGui::SameLine(label_width + style.ItemInnerSpacing.x);
					ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - style.ItemInnerSpacing.x);
					ImGui::SliderFloat("##Smooth", &config.aimBot.speedFactor, 0.0f, 2.0f, "%.2f");
					ImGui::PopItemWidth();
					ImGui::Checkbox("Ignore Team", &config.aimBot.ignoreTeam);
					ImGui::Checkbox("visible(soon)", &config.aimBot.checkVisible);

					// --- Hitbox Auswahl ---
					static const char* hitboxItems[] = { "Head", "Neck", "Spine" };
					ImGui::Combo("Hitbox", &config.aimBot.hitbox, hitboxItems, IM_ARRAYSIZE(hitboxItems));

					ImGui::Unindent();
				}


			}
			break;

		case MenuOrder::VISUALS:
			if (ImGui::CollapsingHeader("ESP General", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Checkbox("Enable ESP", &config.activeCheats.esp);
				if (config.activeCheats.esp)
				{
					ImGui::Indent();
					ImGui::Checkbox("Show Teammates", &config.esp.renderFriendlies);
					ImGui::Checkbox("Bone ESP", &config.esp.bone);
					ImGui::Checkbox("Name ESP", &config.esp.name);
					ImGui::Checkbox("Box ESP", &config.esp.box);
					if (config.esp.box)
					{
						ImGui::Indent();
						ImGui::Checkbox("Box Glow", &config.esp.boxGlow);
						ImGui::Unindent();
					}
					ImGui::Unindent();
				}
			}
			ImGui::Separator();

			if (config.activeCheats.esp)
			{
				if (ImGui::CollapsingHeader("ESP Colors", ImGuiTreeNodeFlags_DefaultOpen))
				{

					float color_label_width = 180.0f;
					ImGui::Text("Enemy Visible");
					ImGui::SameLine(color_label_width);
					ImGui::ColorEdit4("##EnemyVisColor", (float*)&config.esp.colorEnemyVisible, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

					ImGui::Text("Enemy Hidden(soon)");
					ImGui::SameLine(color_label_width);
					ImGui::ColorEdit4("##EnemyHidColor", (float*)&config.esp.colorEnemyHidden, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

					if (config.esp.renderFriendlies)
					{
						ImGui::Text("Team Visible");
						ImGui::SameLine(color_label_width);
						ImGui::ColorEdit4("##TeamVisColor", (float*)&config.esp.colorTeamVisible, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

						ImGui::Text("Team Hidden(soon)");
						ImGui::SameLine(color_label_width);
						ImGui::ColorEdit4("##TeamHidColor", (float*)&config.esp.colorTeamHidden, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
					}
				}
			}
			ImGui::Separator();
			if (ImGui::CollapsingHeader("Other Visuals", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Checkbox("Radar(risky)", &config.activeCheats.radar);
				ImGui::Checkbox("Anti-Flash(risky)", &config.activeCheats.antiFlash);
			}
			break;

		case MenuOrder::MISC:
			if (ImGui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Checkbox("FOV Changer", &config.activeCheats.fovChanger);
				if (config.activeCheats.fovChanger)
				{
					ImGui::Indent();
					ImGui::Text("FOV");
					ImGui::SameLine(label_width + style.ItemInnerSpacing.x);
					ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - style.ItemInnerSpacing.x);
					ImGui::SliderInt("##FOVValue", &config.fov.desiredFov, 60, 140);
					ImGui::PopItemWidth();
					ImGui::Unindent();
				}
				ImGui::Checkbox("Bomb Info Panel™", &config.activeCheats.bombTimer);
			}
			break;

		case MenuOrder::CONFIG:
			if (ImGui::CollapsingHeader("Manage Configurations", ImGuiTreeNodeFlags_DefaultOpen))
			{
				float halfWidth = ImGui::GetContentRegionAvail().x * 0.48f;
				ImGui::BeginChild("ConfigListPane", ImVec2(halfWidth, 150), true);
				ImGui::Text("Saved Configs:");
				if (ImGui::ListBoxHeader("##ConfigList", ImVec2(-1.0f, -1.0f)))
				{
					for (size_t i = 0; i < configs.size(); ++i)
					{
						const bool is_selected = i == 0;
						if (ImGui::Selectable(configs[i].c_str(), is_selected))
							strncpy_s(configNameBuffer, sizeof(configNameBuffer), configs[i].c_str(), _TRUNCATE);

						//if (is_selected)
							//ImGui::SetItemDefaultFocus();
					}
					ImGui::ListBoxFooter();
				}

				ImGui::EndChild();
				ImGui::SameLine();
				ImGui::BeginChild("ConfigActionsPane", ImVec2(0, 150), true);

				ImGui::InputText("Name", configNameBuffer, IM_ARRAYSIZE(configNameBuffer));

				if (ImGui::Button("Save", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
				{
					set_selected_config(configNameBuffer);
					store_all_config_data(files::folder);
				}
				if (ImGui::Button("Load", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
				{
					load_cheat_config_data(files::folder, std::string_view(configNameBuffer));
				}
				if (ImGui::Button("Delete", ImVec2(ImGui::GetContentRegionAvail().x, 0))) { /*Logik*/ }
				ImGui::EndChild();
			}
			break;
		}
	}
	ImGui::EndChild();
	ImGui::End();
}
