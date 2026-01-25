#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

#include "OverlayBase/overlay_base.hpp"

constexpr float bomb_width = 300;
constexpr float bomb_height = 100;

class c_overlay : public OverlayBase
{
private:
    ImVec2 bombScale;

    ImFont *title_font;
    ImFont *font;
    ImFont *esp_font;
    ImFont *bomb_font;

    bool mouse_over_window(const ImVec2 &mouse_pos);
    bool should_show_fps();
    int get_target_fps();
    int get_quick_exit_key();
    int get_menu_toggle_key();
    float get_style_scale();
    void load_fonts(ImGuiIO &imgui_io);
    void render();

    void draw_menu();
    void draw_bomb();
    void draw_cheats();

public:
    c_overlay();
    ~c_overlay() = default;
};
