#include "../overlay.hpp"

#include "config.hpp"
#include "shared_variables.hpp"

#include "CheatBase/memory.hpp"

c_overlay::c_overlay() : OverlayBase(),
                         //		  //
                         // Window //
                         //		  //
                         bombWindowSize(-1.f, -1.f),

                         //			   //
                         // ImGui stuff //
                         //			   //
                         title_font(nullptr),
                         font(nullptr),
                         esp_font(nullptr),
                         bomb_font(nullptr)
{
}

bool c_overlay::mouse_over_window(const ImVec2 &mouse_pos)
{
    ImVec2 menu_tl = persistentConfig.mainWindow.position;
    ImVec2 menu_br = menu_tl + persistentConfig.mainWindow.size;

    ImVec2 bomb_tl = persistentConfig.bombWindow.position;
    ImVec2 bomb_br = bomb_tl + bombWindowSize;

    const ImRect menu_rect = ImRect(menu_tl, menu_br);
    const ImRect bomb_rect = ImRect(bomb_tl, bomb_br);

    const bool mouse_over_menu = menu_rect.Contains(mouse_pos);
    const bool mouse_over_bomb = (config.activeCheats.bombTimer && bomb_rect.Contains(mouse_pos));

    return mouse_over_menu || mouse_over_bomb;
}

bool c_overlay::should_show_fps() { return persistentConfig.showFPS; }
int c_overlay::get_target_fps() { return persistentConfig.targetFPS; }
int c_overlay::get_quick_exit_key() { return config.keybinds.quickExit; }
int c_overlay::get_menu_toggle_key() { return config.keybinds.showMenu; }
float c_overlay::get_style_scale() { return persistentConfig.mainWindow.scale; }

void c_overlay::render()
{
    if (memory.connected)
    {
        draw_cheats();

        if (config.activeCheats.bombTimer)
            draw_bomb();
    }

    if (show_menu)
        draw_menu();
}

void c_overlay::on_reload(ImGuiIO &imgui_io)
{
    const auto font_path = "C:\\Windows\\Fonts\\Tahoma.ttf";

    const auto Fonts = imgui_io.Fonts;

    const auto menu_scale = persistentConfig.mainWindow.scale;
    const auto bomb_scale = persistentConfig.bombWindow.scale;
    const auto esp_scale = persistentConfig.espFontScale;

    font = Fonts->AddFontFromFileTTF(font_path, 16.0f * menu_scale);
    title_font = Fonts->AddFontFromFileTTF(font_path, 20.0f * menu_scale);
    esp_font = Fonts->AddFontFromFileTTF(font_path, 14.0f * esp_scale);
    bomb_font = Fonts->AddFontFromFileTTF(font_path, 25.0f * bomb_scale);

    bombWindowSize = defaultBombWindowSize * bomb_scale;
}
