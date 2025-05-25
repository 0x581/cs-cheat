#include "../overlay.h"

#include <Windows.h>

#include <map>
#include <string>
#include <thread>
#include <vector>


#include <colors.h>
#include <config.h>
#include <keyboard.h>
#include <shared_variables.h>

#include "../window_handle.h"
#include <hotkeys.h>

static bool editing_hotkey_last_frame = false;


using std::string, std::vector, std::map;

// microsoft safe release of pointers
template <typename T>
inline VOID c_overlay::safe_release(T*& p)
{
	if (NULL != p)
	{
		p->Release();
		p = NULL;
	}
}


c_overlay::c_overlay() :
	//		  //
	// Window //
	//		  //
	window_handle(nullptr),
	window_width((FLOAT)GetSystemMetrics(SM_CXSCREEN)),
	window_height((FLOAT)GetSystemMetrics(SM_CYSCREEN)),
	window_scale_changed(false),
	bombScale(bomb_width* persistentConfig.bombWindow.scale, bomb_height* persistentConfig.bombWindow.scale),
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
	font(nullptr),
	titleFont(nullptr),
	//				//
	// Flow control //
	//				//
	show_menu(true),
	exit(false),
	ready_to_close(false)
{
	if (!getWindowHandle(&window_handle))
		throw std::runtime_error("Failed to acquire window handle.");

	if (!init_input_hook())
		throw std::runtime_error("Failed to create keyboard hook.");

	if (!init_device())
		throw std::runtime_error("Failed to initialize DX device.");

	init_imgui();

	if (!update_wnd_proc(window_handle))
		throw std::runtime_error("Failed to change the WndProc Handler.");

	std::thread(&c_overlay::state_handler, this).detach();
}

c_overlay::~c_overlay()
{
	clear_screen();

	dest_imgui();
	dest_device();

	clear_input_hook();
}


BOOL c_overlay::init_device()
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
	D3D_FEATURE_LEVEL vFeatureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };


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
		&device_context
	);

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

	ID3D11BlendState* blendState = nullptr;
	result = d3d_device->CreateBlendState(&blendDesc, &blendState);
	if (FAILED(result))
		return FALSE;

	// Set up rasterizer state
	D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.MultisampleEnable = TRUE;
	rasterDesc.AntialiasedLineEnable = TRUE;

	ID3D11RasterizerState* rasterState = nullptr;
	result = d3d_device->CreateRasterizerState(&rasterDesc, &rasterState);
	if (FAILED(result))
		return FALSE;

	device_context->RSSetState(rasterState);

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	device_context->OMSetBlendState(blendState, blendFactor, 0xFFFFFFFF);


	blendState->Release();
	rasterState->Release();

	init_render_target();

	ShowWindow(window_handle, SW_SHOWNORMAL);
	UpdateWindow(window_handle);
	return TRUE;
}

VOID c_overlay::dest_device()
{
	dest_render_target();
	safe_release(swap_chain);
	safe_release(device_context);
	safe_release(d3d_device);
}


VOID c_overlay::init_render_target()
{
	ID3D11Texture2D* back_buffer = nullptr;
	swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
	if (back_buffer)
		d3d_device->CreateRenderTargetView(back_buffer, NULL, &render_target_view);
	// todo: error here if it can't get the backbuffer of the render target
	back_buffer->Release();
}

VOID c_overlay::dest_render_target()
{
	safe_release(render_target_view);
}


VOID c_overlay::update_on_resolution_change()
{
	//RECT rect{ 0 };
	//GetClientRect(window_handle, &rect);
	//SetWindowPos(window_handle, HWND_TOPMOST, rect.left, rect.top, rect.right, rect.bottom, SWP_NOREDRAW);
}


const void c_overlay::render()
{
	if (exit)
	{
		ready_to_close = true;
		return;
	}

	update_on_resolution_change();

	init_draw_list();

	string name = "Merkur Cheats";
	text({ window_width - ImGui::CalcTextSize(name.c_str()).x - 5, 5 }, name, rgbColors::off_white, true);

	if (persistentConfig.showFPS)
	{
		const auto& io = ImGui::GetIO();

		static float time_since_last_update = 0.f;
		static std::string fps_string{ };

		if ((time_since_last_update += io.DeltaTime) > 0.25f)
		{
			fps_string = std::to_string((int)io.Framerate);
			time_since_last_update = 0.f;
		}

		text({ 5, 5 }, fps_string, rgbColors::off_white, true);
	}

	draw_cheats();

	if (config.activeCheats.bombTimer)
		draw_bomb();

	if (show_menu)
		draw_menu();

	editing_hotkey_last_frame = editing_hotkey;
	editing_hotkey = false;
}

void c_overlay::HandleInput()
{

	constexpr auto set_focus = [](HWND window_handle, bool focus) {
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

	ImVec2 menu_tl = persistentConfig.mainWindow.position;
	menu_tl + persistentConfig.mainWindow.size;
	ImVec2 menu_br = menu_tl + persistentConfig.mainWindow.size;

	ImVec2 bomb_tl = persistentConfig.bombWindow.position;
	ImVec2 bomb_br = ImVec2(bomb_tl.x + bombScale.x, bomb_tl.y + bombScale.y);

	POINT p{};
	GetCursorPos(&p);

	const bool mouse_over_menu = (
		p.x >= menu_tl.x
		&& p.y >= menu_tl.y
		&& p.x < menu_br.x
		&& p.y < menu_br.y
		);

	const bool mouse_over_bomb = (
		config.activeCheats.bombTimer
		&& p.x >= bomb_tl.x
		&& p.y >= bomb_tl.y
		&& p.x < bomb_br.x
		&& p.y < bomb_br.y
		);

	const bool want_mouse = ImGui::GetIO().WantCaptureMouse;

	set_focus(window_handle, mouse_over_menu || mouse_over_bomb || want_mouse);
}


BOOL c_overlay::msg_loop()
{
	if (window_handle)
	{
		HandleInput();

		if (window_scale_changed)
		{
			update_style();
			bombScale = ImVec2(bomb_width * persistentConfig.bombWindow.scale, bomb_height * persistentConfig.bombWindow.scale);
			window_scale_changed = false;
		}

		MSG msg{ 0 };

		if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		render();

		ImGui::Render();
		device_context->OMSetRenderTargets(1, &render_target_view, NULL);
		clear_screen();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		swap_chain->Present(0, 0);

		return msg.message != WM_QUIT;
	}
	return false;
}

VOID c_overlay::clear_screen()
{
	constexpr ImVec4 clear_clr = { 0,0,0,0 };

	device_context->ClearRenderTargetView(render_target_view, (float*)&clear_clr);
}


VOID c_overlay::state_handler()
{
	bool menu_key_held_last_frame = false;
	bool quick_exit_held_last_frame = false;

	for (; !exit;)
	{
		Sleep(1);

		if (editing_hotkey_last_frame)
		{
			menu_key_held_last_frame = true;
			quick_exit_held_last_frame = true;
			continue;
		}

		// panic key
		if (IsKeyPressed(config.keybinds.quickExit))
		{
			if (!quick_exit_held_last_frame)
				exit = true;

			quick_exit_held_last_frame = true;
		}
		else
			quick_exit_held_last_frame = false;

		// Show/Hide menu
		if (IsKeyPressed(config.keybinds.showMenu))
		{
			if (!menu_key_held_last_frame)
				show_menu = !show_menu;

			menu_key_held_last_frame = true;
		}
		else
			menu_key_held_last_frame = false;
	}


	clear_input_hook();
}

VOID c_overlay::init_draw_list()
{
	draw_list = ImGui::GetBackgroundDrawList();
}
