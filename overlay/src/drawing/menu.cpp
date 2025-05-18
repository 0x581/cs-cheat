#include "../../overlay.h"

#include <config.h>
#include <files.h>

static int filter_chars(ImGuiInputTextCallbackData* data)
{
	ImWchar c = data->EventChar;

	const bool valid_char = (
		(c >= 'a' && c <= 'z')
		|| (c >= 'A' && c <= 'Z')
		|| (c >= '0' && c <= '9')
		|| (c == '_')
		|| (c == '-')
		|| (c == ' ')
		);

	return valid_char ? 0 : 1;
}

enum MenuOrder
{
	AIM,
	VISUALS,
	MISC,
	CONFIG,
};

constexpr const auto nav_items = std::to_array({
	"Aim",
	"Visuals",
	"Misc",
	"Config",
	});

static int current_main_nav_item = CONFIG;

constexpr float label_width = 80.0f;


static float calc_slider_start(const ImGuiStyle& style)
{
	const auto lbl1_length = (
		style.WindowPadding.x
		+ style.ItemInnerSpacing.x
		+ ImGui::CalcTextSize("Shot Delay").x
		);

	const auto lbl2_length = (
		style.WindowPadding.x
		+ style.IndentSpacing
		+ style.ItemInnerSpacing.x
		+ ImGui::CalcTextSize("Smooth").x
		);

	return max(lbl1_length, lbl2_length);
}


static void draw_navbar_submenu(const ImGuiStyle& style, ImFont* title_font)
{
	ImGui::BeginChild("NavBar", ImVec2(0, 0), ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar);
	{
		ImGui::PushFont(title_font);
		ImGui::Text("Merkur");
		ImGui::PopFont();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, style.FramePadding.y * 2));

		for (int i = 0; i < nav_items.size(); ++i)
		{
			bool is_selected = (current_main_nav_item == i);
			if (is_selected)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_TabActive]);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, style.Colors[ImGuiCol_TabActive]);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, style.Colors[ImGuiCol_TabActive]);
			}


			if (ImGui::Button(nav_items.at(i), ImVec2(-1, 0)))
				current_main_nav_item = i;

			if (is_selected)
				ImGui::PopStyleColor(3);

			ImGui::Spacing();
		}

		ImGui::PopStyleVar(1);
	}
	ImGui::EndChild();
}

static void draw_aim_submenu(const ImGuiStyle& style, const ImFont* font, const float slider_indent)
{
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

					ImGui::AlignTextToFramePadding();
					ImGui::Text("Shot Delay");
					ImGui::SameLine(slider_indent);
					ImGui::PushItemWidth(-1);
					ImGui::SliderInt("##RageShotDelay", &config.rageBot.shotDelay, 0, 200, "%d ms");
					ImGui::PopItemWidth();

				}
			}
			ImGui::Indent();

			ImGui::Checkbox("Draw FOV Circle", &config.aimBot.drawFovCircle);
			if (config.aimBot.drawFovCircle)
			{
				ImGui::SameLine(0, style.ItemInnerSpacing.x * 2);
				ImGui::ColorEdit4("##FovCircleColor", (float*)&config.aimBot.fovCircleColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
			}
			ImGui::AlignTextToFramePadding();
			ImGui::Text("FOV");
			ImGui::SameLine(slider_indent);
			ImGui::PushItemWidth(-1);
			ImGui::SliderFloat("##AimbotRadius", &config.aimBot.pixelRadius, 0.0f, 400.0f, "%.0fpx");
			ImGui::PopItemWidth();

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Smooth");
			ImGui::SameLine(slider_indent);
			ImGui::PushItemWidth(-1);
			ImGui::SliderFloat("##Smooth", &config.aimBot.speedFactor, 0.0f, 2.0f, "%.2f");
			ImGui::PopItemWidth();

			ImGui::Checkbox("Ignore Team", &config.aimBot.ignoreTeam);
			ImGui::Checkbox("visible (soon)", &config.aimBot.checkVisible);

			// --- Hitbox Auswahl ---
			static const char* hitboxItems[] = { "Head", "Neck", "Spine" };

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Hitbox");
			ImGui::SameLine(slider_indent);
			ImGui::PushItemWidth(-1);
			ImGui::Combo("##Hitbox", &config.aimBot.hitbox, hitboxItems, IM_ARRAYSIZE(hitboxItems));
			ImGui::PopItemWidth();

			ImGui::Unindent();
		}
	}
}

static void draw_visuals_submenu(const ImGuiStyle& style)
{

	if (ImGui::CollapsingHeader("ESP General", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Enable ESP", &config.activeCheats.esp);
		if (config.activeCheats.esp)
		{
			ImGui::Indent();
			ImGui::Checkbox("Show Teammates", &config.esp.renderFriendlies);
			ImGui::Checkbox("Bone ESP", &config.esp.bone);
			ImGui::Checkbox("Name ESP", &config.esp.name);
			ImGui::Checkbox("Health Bar", &config.esp.health);
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
			const float lbl1_width = (
				style.WindowPadding.x
				+ style.IndentSpacing
				+ style.ItemInnerSpacing.x
				+ ImGui::CalcTextSize("Enemy Hidden (soon)").x
				);

			const float lbl2_width = (
				style.WindowPadding.x
				+ style.IndentSpacing
				+ style.ItemInnerSpacing.x
				+ ImGui::CalcTextSize("Team Hidden (soon)").x
				);

			const auto color_indent = max(lbl1_width, lbl2_width);

			ImGui::Indent();

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Enemy Visible");
			ImGui::SameLine(color_indent);
			ImGui::ColorEdit4("##EnemyVisColor", (float*)&config.esp.colorEnemyVisible, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Enemy Hidden (soon)");
			ImGui::SameLine(color_indent);
			ImGui::ColorEdit4("##EnemyHidColor", (float*)&config.esp.colorEnemyHidden, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

			if (config.esp.renderFriendlies)
			{
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Team Visible");
				ImGui::SameLine(color_indent);
				ImGui::ColorEdit4("##TeamVisColor", (float*)&config.esp.colorTeamVisible, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

				ImGui::AlignTextToFramePadding();
				ImGui::Text("Team Hidden (soon)");
				ImGui::SameLine(color_indent);
				ImGui::ColorEdit4("##TeamHidColor", (float*)&config.esp.colorTeamHidden, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
			}
			ImGui::Unindent();

		}
	}
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Other Visuals", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Radar (risky)", &config.activeCheats.radar);
		ImGui::Checkbox("Anti-Flash (risky)", &config.activeCheats.antiFlash);
	}
}

static void draw_misc_submenu(const ImGuiStyle& style, const float slider_indent)
{
	if (ImGui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("FOV Changer", &config.activeCheats.fovChanger);
		if (config.activeCheats.fovChanger)
		{
			ImGui::Indent();

			ImGui::AlignTextToFramePadding();
			ImGui::Text("FOV");
			ImGui::SameLine(slider_indent);
			ImGui::PushItemWidth(-1);
			ImGui::SliderInt("##FOV", &config.fov.desiredFov, 60, 140);
			ImGui::PopItemWidth();
			ImGui::Unindent();
		}
		ImGui::Checkbox("Bomb Info Panel™", &config.activeCheats.bombTimer);
	}
}

static void draw_config_submenu(const ImGuiStyle& style, const ImFont* font)
{
	static char configNameBuffer[CONFIG_NAME_MAX_LENGTH + 1] = "";


	if (ImGui::CollapsingHeader("Manage Configurations", ImGuiTreeNodeFlags_DefaultOpen))
	{

		if (ImGui::BeginTable("TopLevelTable", 2, 0))
		{
			const auto height = (
				4 * (font->FontSize + style.FramePadding.y * 2.0f)
				+ (4 - 1) * style.ItemSpacing.y
				+ style.WindowPadding.y * 2.0f
				);

			ImGui::TableSetupColumn("ConfigSplitLeft", ImGuiTableColumnFlags_WidthStretch, 1);
			ImGui::TableSetupColumn("ConfigSplitRight", ImGuiTableColumnFlags_WidthStretch, 1);

			ImGui::TableNextColumn();

			ImGui::BeginChild("ConfigListPane", ImVec2(-1, height), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);

			//ImGui::Text("Saved Configs:");
			if (ImGui::BeginListBox("##ConfigList", ImVec2(-1, -1)))
			{
				for (const auto& config : configs)
					if (ImGui::Selectable(config.c_str()))
						strncpy_s(configNameBuffer, sizeof(configNameBuffer), config.c_str(), _TRUNCATE);

				ImGui::EndListBox();
			}


			ImGui::EndChild();

			ImGui::TableNextColumn();

			ImGui::BeginChild("ConfigActionsPane", ImVec2(-1, height), ImGuiChildFlags_Borders);

			ImGui::InputText("Name", configNameBuffer, IM_ARRAYSIZE(configNameBuffer), ImGuiInputTextFlags_CallbackCharFilter, filter_chars);

			if (ImGui::Button("Save", ImVec2(-1, 0)))
			{
				if (configNameBuffer[0] != '\0')
				{
					set_selected_config(configNameBuffer);
					store_all_config_data(files::folder);
				}
			}
			if (ImGui::Button("Load", ImVec2(-1, 0)))
			{
				if (configNameBuffer[0] != '\0')
					load_cheat_config_data(files::folder, std::string_view(configNameBuffer));
			}
			if (ImGui::Button("Delete", ImVec2(-1, 0)))
			{
				if (configNameBuffer[0] != '\0')
					delete_config(files::folder, std::string_view(configNameBuffer));
			}
			ImGui::EndChild();

			ImGui::EndTable();
		}
	}
}


void c_overlay::draw_menu()
{
	ImGui::SetCurrentContext(imguiContext);

	const ImGuiStyle& style = ImGui::GetStyle();

	ImGui::SetNextWindowPos(persistentConfig.mainWindow.windowPosition, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(menu_width, menu_height), ImGuiCond_FirstUseEver);

	ImGui::Begin("Main GUI",
		nullptr,
		ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoSavedSettings
	);

	persistentConfig.mainWindow.windowPosition = ImGui::GetWindowPos();

	if (ImGui::BeginTable("TopLevelTable", 2, 0))
	{
		ImGui::TableSetupColumn("TopLevelLeft", ImGuiTableColumnFlags_WidthStretch, 1);
		ImGui::TableSetupColumn("TopLevelRight", ImGuiTableColumnFlags_WidthStretch, 4);

		ImGui::TableNextColumn();

		draw_navbar_submenu(style, titleFont);


		ImGui::TableNextColumn();

		ImGui::BeginChild("ContentArea", ImVec2(0, 0), true);
		{
			const auto slider_indent = calc_slider_start(style);

			ImGui::PushFont(titleFont);
			ImGui::Text("%s Settings", nav_items.at(current_main_nav_item));
			ImGui::PopFont();

			ImGui::Separator();
			ImGui::Spacing();

			switch (current_main_nav_item)
			{
			case MenuOrder::AIM:
				draw_aim_submenu(style, font, slider_indent);
				break;

			case MenuOrder::VISUALS:
				draw_visuals_submenu(style);
				break;

			case MenuOrder::MISC:
				draw_misc_submenu(style, slider_indent);
				break;

			case MenuOrder::CONFIG:
				draw_config_submenu(style, font);
				break;
			}
		}
		ImGui::EndChild();

		ImGui::EndTable();
	}

	ImGui::End();
}
