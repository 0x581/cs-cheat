#include "overlay_base.hpp"

#include <chrono>
#include <map>
#include <string>
#include <thread>
#include <vector>

#include <Windows.h>

#include "colors.hpp"
#include "hotkey_edit.hpp"
#include "keyboard.hpp"

void OverlayBase::render_merkur_info()
{
    constexpr std::string_view name = "Merkur Cheats";

    ImGui::PushFont(info_font);
    const ImVec2 name_size = ImGui::CalcTextSize(name.data());
    ImGui::PopFont();

    text({window_width - name_size.x - 5, 5}, std::string(name), rgbColors::off_white, info_font, true);
}

void OverlayBase::render_fps_info()
{
    const auto &io = ImGui::GetIO();

    static float time_since_last_update = 0.f;
    static std::string fps_string{};

    if ((time_since_last_update += io.DeltaTime) > 1.f)
    {
        fps_string = std::to_string((int)io.Framerate);
        time_since_last_update = 0.f;
    }

    text({5, 5}, fps_string, rgbColors::off_white, info_font, true);
}

void OverlayBase::limit_fps()
{
    static auto next = std::chrono::high_resolution_clock::now();
    static bool fps_was_limited_last_frame = false;

    const int target_fps = get_target_fps();

    // Handle FPS
    if (target_fps == 0)
        fps_was_limited_last_frame = false;
    else
    {
        const auto frame_time = std::chrono::microseconds((int)(1000000.f / target_fps));

        if (!fps_was_limited_last_frame)
            next = std::chrono::high_resolution_clock::now();

        fps_was_limited_last_frame = true;

        next += frame_time;

        std::this_thread::sleep_until(next);
    }
}

void OverlayBase::clear_screen()
{
    constexpr ImVec4 clear_clr = {0, 0, 0, 0};

    device_context->ClearRenderTargetView(render_target_view, (float *)&clear_clr);
}

void OverlayBase::init_draw_list()
{
    draw_list = ImGui::GetBackgroundDrawList();
}

bool OverlayBase::msg_loop()
{
    if (!window_handle)
        return false;

    limit_fps();

    if (style_reload_required)
    {
        reset_style();
        style_reload_required = false;
    }

    handle_focus();
    handle_keybinds();

    update_keybinds();

    MSG msg{0};

    while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);

        if (msg.message == WM_QUIT)
            return false;
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    init_draw_list();

    render_merkur_info();

    if (should_show_fps())
        render_fps_info();

    render();

    editing_hotkey_last_frame = editing_hotkey;
    editing_hotkey = false;

    ImGui::Render();
    device_context->OMSetRenderTargets(1, &render_target_view, NULL);
    clear_screen();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    swap_chain->Present(0, 0);

    return msg.message != WM_QUIT;
}
