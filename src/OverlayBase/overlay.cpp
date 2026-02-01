#include "overlay_base.hpp"

#include <Windows.h>

#include <map>
#include <string>
#include <thread>
#include <vector>

#include "colors.hpp"
#include "keyboard.hpp"

#include "WindowHandle/window_handle.hpp"

using std::string, std::vector, std::map;

// microsoft safe release of pointers
template <typename T>
inline void OverlayBase::safe_release(T *&p)
{
    if (NULL != p)
    {
        p->Release();
        p = NULL;
    }
}

OverlayBase::OverlayBase(Keybind &quick_exit, Keybind &toggle_visibility)
    : //		  //
      // Window //
      //		  //
      window_handle(nullptr),
      window_width((float)GetSystemMetrics(SM_CXSCREEN)),
      window_height((float)GetSystemMetrics(SM_CYSCREEN)),
      style_reload_required(false),
      //			  //
      // DX11 stuff //
      //			  //
      d3d_device(nullptr),
      device_context(nullptr),
      swap_chain(nullptr),
      render_target_view(nullptr),
      //			   //
      // ImGui stuff //
      //			   //
      draw_list(nullptr),
      imguiContext(nullptr),
      info_font(nullptr),
      editing_hotkey_last_frame(false),
      //				//
      // Flow control //
      //				//
      quick_exit_keybind(quick_exit),
      toggle_visibility_keybind(toggle_visibility),
      show_menu(true),
      exit(false)
{
    if (!getWindowHandle(&window_handle))
        throw std::runtime_error("Failed to acquire window handle. Did you run with Administrator privileges?");

    if (!init_device())
        throw std::runtime_error("Failed to initialize DX device.");

    init_imgui();

    if (!update_wnd_proc(window_handle))
        throw std::runtime_error("Failed to change the WndProc Handler.");

    if (timeBeginPeriod(1) != TIMERR_NOERROR)
        throw std::runtime_error("Failed to increase timer resolution.");
}

OverlayBase::~OverlayBase()
{
    clear_screen();

    dest_imgui();
    dest_device();

    timeEndPeriod(1);
}

bool OverlayBase::init_device()
{
    DXGI_SWAP_CHAIN_DESC vSwapChainDesc = {};
    ZeroMemory(&vSwapChainDesc, sizeof(vSwapChainDesc));
    vSwapChainDesc.BufferCount = 2;
    vSwapChainDesc.BufferDesc.Width = 0;
    vSwapChainDesc.BufferDesc.Height = 0;
    vSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    vSwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
    vSwapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
    vSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // | DXGI_SWAP_CHAIN_FLAG_FOREGROUND_LAYER;
    vSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    vSwapChainDesc.OutputWindow = window_handle;
    vSwapChainDesc.SampleDesc.Count = 4;
    vSwapChainDesc.SampleDesc.Quality = D3D11_STANDARD_MULTISAMPLE_PATTERN;
    vSwapChainDesc.Windowed = TRUE;
    vSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL vFeatureLevel;
    D3D_FEATURE_LEVEL vFeatureLevelArray[2] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0};

    HRESULT result = D3D11CreateDeviceAndSwapChain(
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        0,
        vFeatureLevelArray,
        2,
        D3D11_SDK_VERSION,
        &vSwapChainDesc,
        &swap_chain,
        &d3d_device,
        &vFeatureLevel,
        &device_context);

    if (FAILED(result))
        return FALSE;

    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    ID3D11BlendState *blendState = nullptr;
    result = d3d_device->CreateBlendState(&blendDesc, &blendState);
    if (FAILED(result))
        return FALSE;

    // Set up rasterizer state
    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.MultisampleEnable = TRUE;
    rasterDesc.AntialiasedLineEnable = TRUE;

    ID3D11RasterizerState *rasterState = nullptr;
    result = d3d_device->CreateRasterizerState(&rasterDesc, &rasterState);
    if (FAILED(result))
        return FALSE;

    device_context->RSSetState(rasterState);

    float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    device_context->OMSetBlendState(blendState, blendFactor, 0xFFFFFFFF);

    blendState->Release();
    rasterState->Release();

    init_render_target();

    ShowWindow(window_handle, SW_SHOWNORMAL);
    UpdateWindow(window_handle);
    return TRUE;
}

void OverlayBase::dest_device()
{
    dest_render_target();
    safe_release(swap_chain);
    safe_release(device_context);
    safe_release(d3d_device);
}

void OverlayBase::init_render_target()
{
    ID3D11Texture2D *back_buffer = nullptr;
    swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
    if (back_buffer)
        d3d_device->CreateRenderTargetView(back_buffer, NULL, &render_target_view);
    // todo: error here if it can't get the backbuffer of the render target
    back_buffer->Release();
}

void OverlayBase::dest_render_target()
{
    safe_release(render_target_view);
}

void OverlayBase::handle_focus()
{
    constexpr auto set_focus = [](HWND window_handle, bool focus)
    {
        static bool was_focussed = !(GetWindowLong(window_handle, GWL_EXSTYLE) & WS_EX_TRANSPARENT);

        if (focus == was_focussed)
            return;

        LONG style = GetWindowLong(window_handle, GWL_EXSTYLE);

        if (focus)
            SetWindowLong(window_handle, GWL_EXSTYLE, style & ~WS_EX_TRANSPARENT);
        else
            SetWindowLong(window_handle, GWL_EXSTYLE, style | WS_EX_TRANSPARENT);

        // std::cout << "Interface is now " << (focus ? "focused" : "unfocused") << std::endl;

        was_focussed = focus;
    };

    if (!show_menu)
    {
        set_focus(window_handle, false);
        return;
    }

    POINT p{};
    GetCursorPos(&p);

    const bool mouse_over_window = this->mouse_over_window({(float)p.x, (float)p.y});
    const bool want_mouse = ImGui::GetIO().WantCaptureMouse;

    set_focus(window_handle, mouse_over_window || want_mouse);
}

void OverlayBase::handle_keybinds()
{
    this->quick_exit_keybind.update();
    this->toggle_visibility_keybind.update();

    // panic key
    if (this->quick_exit_keybind.get())
        exit = true;

    // Show/Hide menu
    if (this->toggle_visibility_keybind.get())
    {
        show_menu = !show_menu;
        // This makes sure that the keybind has to be released before toggling the menu again
        this->toggle_visibility_keybind.awaitKeyRelease();
    }
}

void OverlayBase::run()
{
    reload_window();

    while (msg_loop() && !exit)
        ;

    clear_screen();
}
