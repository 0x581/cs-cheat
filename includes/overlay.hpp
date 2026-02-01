#pragma once

#include "CheatBase/overlay_base.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

constexpr ImVec2 defaultBombWindowSize = ImVec2(300, 100);

class c_overlay : public OverlayBase
{
private:
    ImVec2 bombWindowSize;

    ImFont *title_font;
    ImFont *font;
    ImFont *esp_font;
    ImFont *bomb_font;

    bool mouse_over_window(const ImVec2 &mouse_pos);
    void update_keybinds();
    bool should_show_fps();
    int get_target_fps();
    float get_style_scale();
    void on_reload(ImGuiIO &imgui_io);
    void render();

    void draw_menu();
    void draw_bomb();
    void draw_cheats();

public:
    c_overlay();
    ~c_overlay() = default;
};
