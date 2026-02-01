#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

#include "toggleable_keybind.hpp"

class OverlayBase
{
public:
    OverlayBase(Keybind &quick_exit, Keybind &toggle_visibility);
    ~OverlayBase();

    void run();

    void reload_window() { style_reload_required = true; }

protected:
    // This function should return true if you want the overlay to capture focus (I.e. mouse is over the window).
    virtual bool mouse_over_window(const ImVec2 &mouse_pos) { return false; }
    // This function should return true if the overlay should display the fps.
    virtual bool should_show_fps() { return true; }
    // In this function, you should update all your keybinds. It gets called once per frame.
    virtual void update_keybinds() {};
    // In this function, you should return the Overlay target fps. Use 0 to disable the limit.
    virtual int get_target_fps() { return 0; }
    // This function should return the scale that should be applied to the ImGui style. Called only when updating the window.
    virtual float get_style_scale() { return 1.0f; }
    // This function should contain all the code that needs to run when reloading the window.
    // This includes scaling the window sizes and loading fonts.
    virtual void on_reload(ImGuiIO &imgui_io) {};
    // This function should contain the render code for your ImGui window.
    virtual void render() {}

private:
    HWND window_handle;

public:
    float window_width;
    float window_height;

private:
    bool style_reload_required;

    ID3D11Device *d3d_device;
    ID3D11DeviceContext *device_context;
    IDXGISwapChain *swap_chain;
    ID3D11RenderTargetView *render_target_view;

    ImDrawList *draw_list;
    ImGuiContext *imguiContext;
    ImFont *info_font;

    bool editing_hotkey_last_frame;

    Keybind &quick_exit_keybind;
    Keybind &toggle_visibility_keybind;

public:
    bool show_menu;
    bool exit;

private:
    template <typename T>
    inline void safe_release(T *&p);

    bool init_device();
    void dest_device();

    void reset_style();

    void init_imgui();
    void dest_imgui();

    void init_render_target();
    void dest_render_target();

    void handle_keybinds();
    void handle_focus();

    void render_fps_info();
    void render_merkur_info();

    void limit_fps();

    void init_draw_list();

    bool msg_loop();

    void clear_screen();

public:
    //					 //
    // Drawing functions //
    //					 //
    void line(const ImVec2 &point1, const ImVec2 &point2, ImU32 color, float line_width);

    void rect(const ImRect &rect, ImU32 color, float line_width, float rounding = 0.f);
    void rect_filled(const ImRect &rect, ImU32 color, float rounding = 0.f);
    void rect_with_glow(const ImRect &rect, ImU32 color, float glow_size, float rounding = 0.f);

    void circle(const ImVec2 &point, float radius, ImU32 color);

    void text(const ImVec2 &pos, const std::string &text, ImU32 color, ImFont *font, bool outline = true);

    inline bool in_screen(const ImVec2 &pos)
    {
        return pos.x >= 0.f && pos.y >= 0.f && pos.x < window_width && pos.y < window_height;
    }
};
