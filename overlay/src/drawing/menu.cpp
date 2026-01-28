#include "../../overlay.hpp"

#include "CheatBase/hotkey_edit.hpp"

#include "config.hpp"
#include "files.hpp"

static int filter_chars(ImGuiInputTextCallbackData *data)
{
    ImWchar c = data->EventChar;

    const bool valid_char = ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || (c == '_') || (c == '-') || (c == ' '));

    return valid_char ? 0 : 1;
}

enum MenuOrder
{
    AIM,
    VISUALS,
    MISC,
    CONFIG,
    KEYBINDS,
    SETTINGS,
};

constexpr const auto nav_items = std::to_array({
    std::make_pair("Aim", "Aim Settings"),
    std::make_pair("Visuals", "Visuals Settings"),
    std::make_pair("Misc", "Misc Settings"),
    std::make_pair("Config", "Config Settings"),
    std::make_pair("Keybinds", "Keybinds"),
    std::make_pair("Settings", "Settings"),
});

static int current_main_nav_item = AIM;

static float calc_slider_start(const ImGuiStyle &style)
{
    const auto lbl1_length = (style.WindowPadding.x + style.ItemInnerSpacing.x + ImGui::CalcTextSize("Shot Delay").x);

    const auto lbl2_length = (style.WindowPadding.x + style.IndentSpacing + style.ItemInnerSpacing.x + ImGui::CalcTextSize("Smooth").x);

    return max(lbl1_length, lbl2_length);
}

static void draw_navbar_submenu(const ImGuiStyle &style, ImFont *title_font, ImFont *text_font)
{
    constexpr auto buttons_at_bottom = 2;

    ImGui::BeginChild("NavBar", ImVec2(0, 0), ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar);
    {
        ImGui::PushFont(title_font);
        ImGui::TextUnformatted("Merkur");
        ImGui::PopFont();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, style.FramePadding.y * 2));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x, style.ItemSpacing.y * 2));

        for (int i = 0; i < nav_items.size(); ++i)
        {
            bool is_selected = (current_main_nav_item == i);
            if (is_selected)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_TabActive]);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, style.Colors[ImGuiCol_TabActive]);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, style.Colors[ImGuiCol_TabActive]);
            }

            if (i == nav_items.size() - buttons_at_bottom)
            {
                const auto push_height = (ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeight() * buttons_at_bottom - style.ItemSpacing.y * (buttons_at_bottom - 1));

                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + push_height);
            }

            if (ImGui::Button(nav_items.at(i).first, ImVec2(-1, 0)))
                current_main_nav_item = i;

            if (is_selected)
                ImGui::PopStyleColor(3);
        }

        ImGui::PopStyleVar(2);
    }
    ImGui::EndChild();
}

static void draw_aim_submenu(const ImGuiStyle &style, const ImFont *font, const float slider_indent)
{
    if (ImGui::CollapsingHeader("Aimbot Options", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Triggerbot", &config.activeCheats.triggerBot);
        ImGui::Checkbox("Enable Aimbot", &config.activeCheats.aimBot);
        if (config.activeCheats.aimBot)
        {
            if (ImGui::CollapsingHeader("Ragebot Options", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (config.devSettings.devMode)
                {
                    ImGui::Checkbox("Enable Ragebot", &config.activeCheats.rageBot);
                    if (config.activeCheats.rageBot)
                    {

                        ImGui::TextUnformatted("Ragebot use the aimbot funktions.");

                        ImGui::AlignTextToFramePadding();
                        ImGui::TextUnformatted("Shot Delay");
                        ImGui::SameLine(slider_indent);
                        ImGui::PushItemWidth(-1);
                        ImGui::SliderInt("##RageShotDelay", &config.rageBot.shotDelay, 0, 200, "%d ms", ImGuiSliderFlags_ClampOnInput);
                        ImGui::PopItemWidth();
                    }
                }
            }
            ImGui::Indent();

            ImGui::Checkbox("Draw FOV Circle", &config.aimBot.drawFovCircle);
            if (config.aimBot.drawFovCircle)
            {
                static int currentKey = -1;

                ImGui::SameLine(0, style.ItemInnerSpacing.x * 2);
                ImGui::ColorEdit4("##FovCircleColor", (float *)&config.aimBot.fovCircleColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
            }
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("FOV");
            ImGui::SameLine(slider_indent);
            ImGui::PushItemWidth(-1);
            ImGui::SliderFloat("##AimbotRadius", &config.aimBot.pixelRadius, 0.0f, 400.0f, "%.0fpx", ImGuiSliderFlags_ClampOnInput);
            ImGui::PopItemWidth();

            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Smooth");
            ImGui::SameLine(slider_indent);
            ImGui::PushItemWidth(-1);
            ImGui::SliderFloat("##Smooth", &config.aimBot.speedFactor, 0.0f, 2.0f, "%.2f", ImGuiSliderFlags_ClampOnInput);
            ImGui::PopItemWidth();

            ImGui::Checkbox("Ignore Team", &config.aimBot.ignoreTeam);
            ImGui::Checkbox("visible (soon)", &config.aimBot.checkVisible);

            // --- Hitbox Auswahl ---
            static const char *hitboxItems[] = {"Head", "Neck", "Spine"};

            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Hitbox");
            ImGui::SameLine(slider_indent);
            ImGui::PushItemWidth(-1);
            ImGui::Combo("##Hitbox", &config.aimBot.hitbox, hitboxItems, IM_ARRAYSIZE(hitboxItems));
            ImGui::PopItemWidth();

            ImGui::Unindent();
        }
    }
}

static void draw_visuals_submenu(const ImGuiStyle &style, const float slider_indent)
{

    if (ImGui::CollapsingHeader("ESP General", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Enable ESP", &config.activeCheats.esp);
        if (config.activeCheats.esp)
        {
            ImGui::Indent();
            ImGui::Checkbox("Show Teammates", &config.esp.renderFriendlies);
            ImGui::Checkbox("Bone ESP", &config.esp.bone);
            if (config.esp.bone)
            {
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted("Width");
                ImGui::SameLine(slider_indent);
                ImGui::PushItemWidth(-1);
                ImGui::SliderFloat("##EspBoneWidth", &config.esp.boneWidth, 0.5f, 10.0f, "%.2fx", ImGuiSliderFlags_ClampOnInput);
                ImGui::PopItemWidth();
            }
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
            const float lbl1_width = (style.WindowPadding.x + style.IndentSpacing + style.ItemInnerSpacing.x + ImGui::CalcTextSize("Enemy Hidden (soon)").x);

            const float lbl2_width = (style.WindowPadding.x + style.IndentSpacing + style.ItemInnerSpacing.x + ImGui::CalcTextSize("Team Hidden (soon)").x);

            const auto color_indent = max(lbl1_width, lbl2_width);

            ImGui::Indent();

            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Enemy Visible");
            ImGui::SameLine(color_indent);
            ImGui::ColorEdit4("##EnemyVisColor", (float *)&config.esp.colorEnemyVisible, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Enemy Hidden (soon)");
            ImGui::SameLine(color_indent);
            ImGui::ColorEdit4("##EnemyHidColor", (float *)&config.esp.colorEnemyHidden, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

            if (config.esp.renderFriendlies)
            {
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted("Team Visible");
                ImGui::SameLine(color_indent);
                ImGui::ColorEdit4("##TeamVisColor", (float *)&config.esp.colorTeamVisible, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted("Team Hidden (soon)");
                ImGui::SameLine(color_indent);
                ImGui::ColorEdit4("##TeamHidColor", (float *)&config.esp.colorTeamHidden, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
            }
            ImGui::Unindent();
        }
    }
    ImGui::Separator();
    if (config.devSettings.devMode)
    {
        if (ImGui::CollapsingHeader("Other Visuals", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Checkbox("Radar (risky)", &config.activeCheats.radar);
            ImGui::Checkbox("Anti-Flash (risky)", &config.activeCheats.antiFlash);
        }
    }
}

static void draw_misc_submenu(const ImGuiStyle &style, const float slider_indent)
{
    if (ImGui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (config.devSettings.devMode)
        {
            ImGui::Checkbox("FOV Changer", &config.activeCheats.fovChanger);
            if (config.activeCheats.fovChanger)
            {
                ImGui::Indent();

                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted("FOV");
                ImGui::SameLine(slider_indent);
                ImGui::PushItemWidth(-1);
                ImGui::SliderInt("##FOV", &config.fov.desiredFov, 60, 140, "%d", ImGuiSliderFlags_ClampOnInput);
                ImGui::PopItemWidth();
                ImGui::Unindent();
            }
        }

        ImGui::Checkbox("Bomb Info Panel�", &config.activeCheats.bombTimer);
    }
}

static void draw_config_submenu(const ImGuiStyle &style, const ImFont *font)
{
    static char configNameBuffer[configManager.MAX_CONFIG_NAME_LENGTH + 1] = "";
    static bool first = true;
    if (first)
    {
        strncpy_s(configNameBuffer, sizeof(configNameBuffer), persistentConfig.selectedConfig.c_str(), _TRUNCATE);
        first = false;
    }

    if (ImGui::CollapsingHeader("Manage Configurations", ImGuiTreeNodeFlags_DefaultOpen))
    {

        if (ImGui::BeginTable("TopLevelTable", 2, 0))
        {
            const auto height = (4 * ImGui::GetFrameHeight() + (4 - 1) * style.ItemSpacing.y + style.WindowPadding.y * 2.0f);

            ImGui::TableSetupColumn("ConfigSplitLeft", ImGuiTableColumnFlags_WidthStretch, 1);
            ImGui::TableSetupColumn("ConfigSplitRight", ImGuiTableColumnFlags_WidthStretch, 1);

            ImGui::TableNextColumn();

            ImGui::BeginChild("ConfigListPane", ImVec2(-1, height), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);

            if (ImGui::BeginListBox("##ConfigList", ImVec2(-1, -1)))
            {
                for (const auto &config : configManager.configs)
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
                    if (configManager.set_config_name(configNameBuffer))
                        configManager.store_all_config_data();
                }
            }
            if (ImGui::Button("Load", ImVec2(-1, 0)))
            {
                if (configNameBuffer[0] != '\0')
                    configManager.load_cheat_config(configNameBuffer);
            }
            if (ImGui::Button("Delete", ImVec2(-1, 0)))
            {
                if (configNameBuffer[0] != '\0')
                    configManager.delete_config(configNameBuffer);
            }
            ImGui::EndChild();

            ImGui::EndTable();
        }
        ImGui::Checkbox("Dev mode", &config.devSettings.devMode);
        if (config.devSettings.devMode)
        {
            bool allOn = false;
            bool allOff = false;

            ImGui::TextUnformatted("Dev mode is enabled, some features may not work as intended.");
            ImGui::Checkbox("All On", &allOn);
            ImGui::Checkbox("All Off", &allOff);
            if (allOn)
            {
                config.activeCheats.aimBot = true;
                config.activeCheats.rageBot = true;
                config.esp.bone = true;
                config.esp.name = true;
                config.esp.box = true;
                config.esp.health = true;
                config.esp.renderFriendlies = true;
                config.esp.boxGlow = true;
                config.aimBot.drawFovCircle = true;
                config.aimBot.ignoreTeam = true;
                config.aimBot.checkVisible = true;
                config.activeCheats.esp = true;
                config.activeCheats.triggerBot = true;
                config.activeCheats.radar = true;
                config.activeCheats.antiFlash = true;
                config.activeCheats.bombTimer = true;
                config.activeCheats.fovChanger = true;
            }
            if (allOff)
            {
                config.activeCheats.aimBot = false;
                config.activeCheats.rageBot = false;
                config.esp.bone = false;
                config.esp.name = false;
                config.esp.box = false;
                config.esp.health = false;
                config.esp.renderFriendlies = false;
                config.esp.boxGlow = false;
                config.aimBot.drawFovCircle = false;
                config.aimBot.ignoreTeam = false;
                config.aimBot.checkVisible = false;
                config.activeCheats.esp = false;
                config.activeCheats.triggerBot = false;
                config.activeCheats.radar = false;
                config.activeCheats.antiFlash = false;
                config.activeCheats.bombTimer = false;
                config.activeCheats.fovChanger = false;
            }
        }
    }
}

static void draw_keybinds_submenu(const ImGuiStyle &style)
{
    if (ImGui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen))
    {
        HotkeyEdit("Quick Exit", &config.keybinds.quickExit);
        HotkeyEdit("Toggle Menu", &config.keybinds.showMenu);
    }

    if (ImGui::CollapsingHeader("Aimbot", ImGuiTreeNodeFlags_DefaultOpen))
    {
        HotkeyEdit("Aimbot", config.keybinds.aimBot);
        HotkeyEdit("Triggerbot", config.keybinds.triggerBot);
        HotkeyEdit("Ragebot", config.keybinds.rageBot);
    }

    if (config.devSettings.devMode && ImGui::CollapsingHeader("Other", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::TextUnformatted("Dev mode is enabled, some features may not work as intended.");
        HotkeyEdit("BunnyHop", config.keybinds.bunnyHop);
    }
}

static void draw_settings_submenu(const ImGuiStyle &style, const float slider_indent, c_overlay &overlay)
{
    if (ImGui::CollapsingHeader("Scaling", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Menu");
        ImGui::SameLine(slider_indent);
        ImGui::PushItemWidth(-1);
        bool value_changed = ImGui::SliderFloat(
            "##MainWindowScale",
            &persistentConfig.mainWindow.scale,
            0.5,
            3.0,
            "%.2fx",
            ImGuiSliderFlags_ClampOnInput);
        ImGui::PopItemWidth();

        if (ImGui::IsItemDeactivatedAfterEdit())
            overlay.reload_window();

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Bomb");
        ImGui::SameLine(slider_indent);
        ImGui::PushItemWidth(-1);
        value_changed = ImGui::SliderFloat(
            "##BombWindowScale",
            &persistentConfig.bombWindow.scale,
            0.5,
            3.0,
            "%.2fx",
            ImGuiSliderFlags_ClampOnInput);
        ImGui::PopItemWidth();

        if (ImGui::IsItemDeactivatedAfterEdit())
            overlay.reload_window();

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Names");
        ImGui::SameLine(slider_indent);
        ImGui::PushItemWidth(-1);
        value_changed = ImGui::SliderFloat(
            "##NameFontScale",
            &persistentConfig.espFontScale,
            0.5f,
            3.0f,
            "%.2fx",
            ImGuiSliderFlags_ClampOnInput);
        ImGui::PopItemWidth();

        if (ImGui::IsItemDeactivatedAfterEdit())
            overlay.reload_window();
    }

    if (ImGui::CollapsingHeader("Other", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("FPS");
        ImGui::SameLine(slider_indent);
        ImGui::PushItemWidth(-1);
        const bool value_changed = ImGui::SliderInt(
            "##FPS",
            &persistentConfig.targetFPS,
            59,
            1000,
            persistentConfig.targetFPS ? "%d" : "Unlimited",
            ImGuiSliderFlags_ClampOnInput);
        ImGui::PopItemWidth();

        if (persistentConfig.targetFPS == 59)
            persistentConfig.targetFPS = 0;

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Limits Maximum FPS. Ctrl click to manually edit.");

        ImGui::Checkbox("Show FPS", &persistentConfig.showFPS);
    }
}

void c_overlay::draw_menu()
{
    const ImGuiStyle &style = ImGui::GetStyle();

    ImGui::SetNextWindowPos(persistentConfig.mainWindow.position, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(persistentConfig.mainWindow.size, ImGuiCond_FirstUseEver);

    ImGui::Begin("Main GUI",
                 nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);

    persistentConfig.mainWindow.position = ImGui::GetWindowPos();
    persistentConfig.mainWindow.size = ImGui::GetWindowSize();

    if (ImGui::BeginTable("TopLevelTable", 2, 0))
    {
        ImGui::TableSetupColumn("TopLevelLeft", ImGuiTableColumnFlags_WidthStretch, 1);
        ImGui::TableSetupColumn("TopLevelRight", ImGuiTableColumnFlags_WidthStretch, 4);

        ImGui::TableNextColumn();

        draw_navbar_submenu(style, title_font, font);

        ImGui::TableNextColumn();

        ImGui::BeginChild("ContentArea", ImVec2(0, 0), true);
        {
            const auto slider_indent = calc_slider_start(style);

            ImGui::PushFont(title_font);
            ImGui::TextUnformatted(nav_items.at(current_main_nav_item).second);
            ImGui::PopFont();

            ImGui::Separator();
            ImGui::Spacing();

            switch (current_main_nav_item)
            {
            case MenuOrder::AIM:
                draw_aim_submenu(style, font, slider_indent);
                break;

            case MenuOrder::VISUALS:
                draw_visuals_submenu(style, slider_indent);
                break;

            case MenuOrder::MISC:
                draw_misc_submenu(style, slider_indent);
                break;

            case MenuOrder::CONFIG:
                draw_config_submenu(style, font);
                break;

            case MenuOrder::KEYBINDS:
                draw_keybinds_submenu(style);
                break;

            case MenuOrder::SETTINGS:
                draw_settings_submenu(style, slider_indent, *this);
                break;
            }
        }
        ImGui::EndChild();

        ImGui::EndTable();
    }

    ImGui::End();
}
