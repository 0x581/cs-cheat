#include "../overlay.h"

#include <string>
#include <vector>
#include <map>

#include <Windows.h>


#include <colors.h>
#include <config.h>
#include <keybinds.h>
#include <shared_variables.h>

#include "../window_handle.h"


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
	ready_to_close(false),
	exit(false),
	window_handle(nullptr),
	window_class({}),
	window_width((FLOAT)GetSystemMetrics(SM_CXSCREEN)),
	window_height((FLOAT)GetSystemMetrics(SM_CYSCREEN)),
	font(nullptr),
	d3d_device(nullptr),
	device_context(nullptr),
	swap_chain(nullptr),
	render_target_view(nullptr),
	draw_list(nullptr),
	imguiContext(nullptr),
	breath(NULL)
{

	if (!getWindowHandle(&window_handle))
		throw std::runtime_error(string("Failed to acquire window handle."));

	if (!init_device())
		throw std::runtime_error(string("Failed to initialize DX device."));

	init_imgui();

	std::thread(&c_overlay::anim_handler, this).detach();
}

c_overlay::~c_overlay()
{
	clear_screen();

	dest_imgui();
	dest_device();
}


BOOL c_overlay::init_device()
{
	DXGI_SWAP_CHAIN_DESC vSwapChainDesc = {};
	ZeroMemory(&vSwapChainDesc, sizeof(vSwapChainDesc));
	vSwapChainDesc.BufferCount = 2;
	vSwapChainDesc.BufferDesc.Width = 0;
	vSwapChainDesc.BufferDesc.Height = 0;
	vSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	vSwapChainDesc.BufferDesc.RefreshRate.Numerator = 240;
	vSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
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


VOID c_overlay::init_imgui()
{
	IMGUI_CHECKVERSION();

	imguiContext = ImGui::CreateContext();
	ImGui::SetCurrentContext(imguiContext);

	auto imgui_io = ImGui::GetIO();

	imgui_io.IniFilename = nullptr;
	imgui_io.LogFilename = nullptr;

	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	// --- Theme Colors with Purple Accent ---
	ImVec4 almostBlack = ImVec4(0.06f, 0.06f, 0.07f, 0.95f);
	ImVec4 darkGray = ImVec4(0.10f, 0.10f, 0.12f, 0.95f);
	// Lila Akzentfarben
	ImVec4 brightPurple = ImVec4(0.5f, 0.2f, 0.8f, 1.00f);      // Ein kräftiges Lila
	ImVec4 brightPurpleHover = ImVec4(0.6f, 0.3f, 0.9f, 1.00f); // Etwas helleres Lila für Hover/Aktiv
	ImVec4 lightText = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
	ImVec4 darkOutline = ImVec4(0.12f, 0.12f, 0.15f, 0.90f);

	// --- General Style Variables ---
	style.WindowPadding = ImVec2(15, 15);
	style.WindowRounding = 5.0f;
	style.FramePadding = ImVec2(8, 4);
	style.FrameRounding = 4.0f;
	style.ItemSpacing = ImVec2(10, 6);
	style.ItemInnerSpacing = ImVec2(6, 6);
	style.IndentSpacing = 20.0f;
	style.ScrollbarSize = 12.0f;
	style.ScrollbarRounding = 9.0f;
	style.GrabMinSize = 10.0f;
	style.GrabRounding = 3.0f;

	style.WindowBorderSize = 1.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupBorderSize = 1.0f;
	style.FrameBorderSize = 0.0f;
	style.TabBorderSize = 0.0f;

	// --- Color Scheme ---
	colors[ImGuiCol_Text] = lightText;
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

	colors[ImGuiCol_WindowBg] = almostBlack;
	colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 0.95f);
	colors[ImGuiCol_PopupBg] = almostBlack;
	colors[ImGuiCol_Border] = brightPurple;           // Fensterrahmenfarbe (Akzent)
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

	colors[ImGuiCol_FrameBg] = darkGray;
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);

	colors[ImGuiCol_TitleBg] = almostBlack;
	colors[ImGuiCol_TitleBgActive] = darkGray;
	colors[ImGuiCol_TitleBgCollapsed] = almostBlack;

	colors[ImGuiCol_MenuBarBg] = darkGray;

	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);

	colors[ImGuiCol_CheckMark] = brightPurple;

	colors[ImGuiCol_SliderGrab] = brightPurple;
	colors[ImGuiCol_SliderGrabActive] = brightPurpleHover;

	colors[ImGuiCol_Button] = darkGray;
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.15f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_ButtonActive] = brightPurple;         // Aktiver Button (z.B. in der NavBar)

	colors[ImGuiCol_Header] = darkGray;           // CollapsingHeader Hintergrund
	colors[ImGuiCol_HeaderHovered] = brightPurpleHover;
	colors[ImGuiCol_HeaderActive] = brightPurple;

	colors[ImGuiCol_Separator] = darkOutline;
	colors[ImGuiCol_SeparatorHovered] = brightPurple; // Kann auch Akzentfarbe sein
	colors[ImGuiCol_SeparatorActive] = brightPurpleHover;

	colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_ResizeGripHovered] = brightPurple; // Thematisiert
	colors[ImGuiCol_ResizeGripActive] = brightPurpleHover; // Thematisiert

	colors[ImGuiCol_Tab] = darkGray;
	colors[ImGuiCol_TabHovered] = ImVec4(0.15f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_TabActive] = brightPurple; // Aktiver Nav-Button
	colors[ImGuiCol_TabUnfocused] = darkGray;
	colors[ImGuiCol_TabUnfocusedActive] = brightPurple;

	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = brightPurpleHover;
	colors[ImGuiCol_PlotHistogram] = brightPurple;
	colors[ImGuiCol_PlotHistogramHovered] = brightPurpleHover;

	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);

	colors[ImGuiCol_TextSelectedBg] = ImVec4(brightPurple.x, brightPurple.y, brightPurple.z, 0.35f); // Lila mit Alpha
	colors[ImGuiCol_DragDropTarget] = brightPurpleHover;
	colors[ImGuiCol_NavHighlight] = brightPurple; // Farbe für Tastaturnavigation
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.04f, 0.04f, 0.05f, 0.65f);

	// --- Font Laden (dein Code) ---
	font = imgui_io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 16.0f * scaling_value);
	titleFont = imgui_io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 20.0f * scaling_value);

	style.ScaleAllSizes(scaling_value);

	ImGui_ImplWin32_Init(window_handle);
	ImGui_ImplDX11_Init(d3d_device, device_context);

	SetWindowLongPtr(window_handle, GWLP_WNDPROC, (LONG_PTR)WndProc_custom);
}

VOID c_overlay::dest_imgui()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext(imguiContext);
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


const void c_overlay::render(FLOAT width, FLOAT height)
{
	if (exit)
	{
		ready_to_close = true;
		return;
	}

	init_draw_list(); // we only do this once a loop

	//#define pos_sin(x) ((sin(x) + 1.) / 2.)
	//	const auto time = ImGui::GetTime();
	//	const auto col1 = IM_COL32(255.f * pos_sin(time + 0.f), 255.f * pos_sin(time + 1.f), 255.f * pos_sin(time + 2.f), 64.f);
	//	const auto col2 = IM_COL32(255.f * pos_sin(time + 1.f), 255.f * pos_sin(time + 2.f), 255.f * pos_sin(time + 3.f), 64.f);
	//	radial_gradient(ImVec2(window_width / 2.f, window_height / 2.f), 400, col2, col1);

	string name = "Merkur Cheats";
	text({ window_width - ImGui::CalcTextSize(name.c_str()).x - 5, 0 }, name, rgbColors::off_white, true);

	renderCheats();

	if (config.activeCheats.bombTimer)
		draw_bomb();

	if (!show_menu)
		return;

	draw_menu();
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

		std::cout << "Interface is now " << (focus ? "focused" : "unfocused") << std::endl;


		was_focussed = focus;
		};


	if (!show_menu)
	{
		set_focus(window_handle, false);
		return;
	}

	ImVec2 menu_tl = persistentConfig.mainWindow.windowPosition;
	ImVec2 menu_br = ImVec2(menu_tl.x + menu_width, menu_tl.y + menu_height);

	ImVec2 bomb_tl = persistentConfig.bombWindow.windowPosition;
	ImVec2 bomb_br = ImVec2(bomb_tl.x + bomb_width, bomb_tl.y + bomb_height);

	POINT p{};
	GetCursorPos(&p);

	bool mouse_over_imgui = (
		p.x >= menu_tl.x
		&& p.y >= menu_tl.y
		&& p.x < menu_br.x
		&& p.y < menu_br.y
		);

	bool mouse_over_bomb = (
		config.activeCheats.bombTimer
		&& p.x >= bomb_tl.x
		&& p.y >= bomb_tl.y
		&& p.x < bomb_br.x
		&& p.y < bomb_br.y
		);

	set_focus(window_handle, mouse_over_imgui || mouse_over_bomb);
}


BOOL c_overlay::msg_loop()
{
	if (window_handle)
	{
		HandleInput();

		MSG msg{ 0 };

		if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		render(window_width, window_height);

		ImGui::Render();
		device_context->OMSetRenderTargets(1, &render_target_view, NULL);
		clear_screen();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		swap_chain->Present(1, 0);

		return msg.message != WM_QUIT;
	}
	return false;
}

void c_overlay::clear_screen()
{
	constexpr ImVec4 clear_clr = { 0,0,0,0 };

	device_context->ClearRenderTargetView(render_target_view, (float*)&clear_clr);
}


void c_overlay::anim_handler()
{
	// timers for our animations
	breath_timer.start();

	bool menuKeyReleased = true;

	for (; !exit;)
	{
		RECT rect{ 0 };
		POINT point{ 0 };
		GetClientRect(window_handle, &rect);
		SetWindowPos(window_handle, HWND_TOPMOST, rect.left, rect.top, rect.right, rect.bottom, SWP_NOREDRAW);

		// panic key
		if (GetAsyncKeyState(keybinds::quickExit) & 0x8000)
			exit = true;

		// timer for our breathing animation
		int breath_tmr = breath_timer.end();
		if (breath_tmr >= breath_anim_time * 2)
		{
			breath = breath_anim_time;
			breath_timer.start();
		}
		else if (breath_tmr > breath_anim_time)
			breath = breath_anim_time - (breath_tmr - breath_anim_time);
		else if (breath_tmr <= breath_anim_time)
			breath = breath_tmr;

		// Show/Hide menu
		if (GetAsyncKeyState(keybinds::showMenu) & 0x8000)
		{
			if (menuKeyReleased)
				show_menu = !show_menu;

			menuKeyReleased = false;
		}
		else
			menuKeyReleased = true;
	}
}

VOID c_overlay::init_draw_list()
{
	draw_list = ImGui::GetBackgroundDrawList();
}
