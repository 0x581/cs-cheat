#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

class OverlayBase
{
public:
    OverlayBase();
    ~OverlayBase();

    void run();

    void reload_window() { style_reload_required = true; }

protected:
    // Functions that the derived class must implement
    virtual bool mouse_over_window(const ImVec2 &mouse_pos) { return false; }
    virtual bool should_show_fps() { return true; }
    virtual int get_target_fps() { return 0; }
    virtual int get_quick_exit_key() { return VK_END; }
    virtual int get_menu_toggle_key() { return VK_INSERT; }
    virtual float get_style_scale() { return 1.0f; }
    virtual void on_reload(ImGuiIO &imgui_io) {};
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
