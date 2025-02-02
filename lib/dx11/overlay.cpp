#include "../pch.h"
#include "../../shared/sharedVariables.hpp"
#include "../../shared/defines.hpp"
#include "../../shared/colors.hpp"
#include "../../config/config.hpp"
#include "../../shared/consoleControl.cpp"

#include <map>


// microsoft safe release of pointers
template <typename T>
inline VOID c_overlay::safe_release(T*& p) {
	if (NULL != p) {
		p->Release();
		p = NULL;
	}
}

static bool l_test = false;

const std::map<std::string_view, const ImU32> availableColors
{
	{"Green", rgbColors::green},
	{"Red", rgbColors::red},
	{"Blue", rgbColors::blue},
	{"Yellow", rgbColors::yellow},
	{"Black", rgbColors::black},
	{"White", rgbColors::white},
	{"Purple", rgbColors::purple1},
	{"Dark Purple", rgbColors::purple2},
};

auto selectedColorIt = availableColors.begin();


ImGuiContext* menuContext = ImGui::CreateContext();


BOOL c_overlay::init_device() {
	
	DXGI_SWAP_CHAIN_DESC vSwapChainDesc = {};
	ZeroMemory(&vSwapChainDesc, sizeof(vSwapChainDesc));
	vSwapChainDesc.BufferCount = 2;
	vSwapChainDesc.BufferDesc.Width = 0;
	vSwapChainDesc.BufferDesc.Height = 0;
	vSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	vSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	vSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	vSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
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

	if (FAILED(result)) {
		return FALSE; 
	}

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
	if (FAILED(result)) {
		return FALSE; 
	}

	// Set up rasterizer state
	D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.MultisampleEnable = TRUE;
	rasterDesc.AntialiasedLineEnable = TRUE;

	ID3D11RasterizerState* rasterState = nullptr;
	result = d3d_device->CreateRasterizerState(&rasterDesc, &rasterState);
	if (FAILED(result)) {
		return FALSE; 
	}

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

VOID c_overlay::dest_device() {
	dest_render_target();
	safe_release(swap_chain);
	safe_release(device_context);
	safe_release(d3d_device);
}

VOID c_overlay::init_imgui() {
	IMGUI_CHECKVERSION();

	ImVec4* colors = ImGui::GetStyle().Colors;

	ImGui::SetCurrentContext(menuContext);
	ImGui::StyleColorsClassic();
	ImGuiStyle& style = ImGui::GetStyle();

	style.WindowTitleAlign.x = 0.50f;
	style.WindowPadding = ImVec2(20, 20);
	style.WindowRounding = 20.0f;
	style.FramePadding = ImVec2(10, 4);
	style.FrameRounding = 12.0f;
	style.ItemSpacing = ImVec2(12, 8);
	style.ItemInnerSpacing = ImVec2(12, 6);
	style.IndentSpacing = 25.0f;
	style.ScrollbarSize = 15.0f;
	style.ScrollbarRounding = 9.0f;
	style.GrabMinSize = 5.0f;
	style.GrabRounding = 3.0f;
	style.WindowBorderSize = 5.0f;

	style.TabBorderSize = 2.0f;
	style.FrameBorderSize = 2.0f;
	style.GrabMinSize = 10.0f;

	colors[ImGuiCol_Border] = ImVec4(0.46f, 0.00f, 0.53f, 0.70f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.96f, 0.01f, 0.01f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.00f, 0.12f, 0.85f);
	//colors[ImGuiCol_Border] = ImVec4(1.00f, 0.00f, 0.00f, 0.30f);
	colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 0.00f, 0.00f, 0.84f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.06f, 0.00f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.05f, 0.00f, 0.12f, 0.85f);
	colors[ImGuiCol_TabHovered] = ImVec4(1.00f, 0.04f, 0.00f, 0.71f);
	colors[ImGuiCol_TabActive] = ImVec4(1.00f, 0.04f, 0.00f, 0.71f);


	//style.Colors[ImGuiCol_Border] = ImVec4(0.012f, 0.91f, 0.957f, ((breath / (float)breath_anim_time) * .7f) + .3f);

	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.043f, 0.043f, 0.043f, 0.73f);

	ImGuiIO& io = ImGui::GetIO();
	font->im_font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 24.0f);
	font->font_size = 24.f;

	ImGui_ImplWin32_Init(window_handle);
	ImGui_ImplDX11_Init(d3d_device, device_context);
}

VOID c_overlay::dest_imgui() {
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

VOID c_overlay::init_render_target() {
	ID3D11Texture2D* back_buffer = nullptr;
	swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
	if (back_buffer)
		d3d_device->CreateRenderTargetView(back_buffer, NULL, &render_target_view);
	// todo: error here if it can't get the backbuffer of the render target
	back_buffer->Release();
}

VOID c_overlay::dest_render_target() {
	if (!render_target_view)
		return;

	render_target_view->Release();
	render_target_view = NULL;
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
	breath(NULL)
{
	// get a handle to the nvidia window
	window_handle = FindWindowA("CEF-OSC-WIDGET", "NVIDIA GeForce Overlay"); //Hijack nvidia share, bc for some reason this is fucking possible?
	auto getInfo = GetWindowLongA(window_handle, -20);
	auto changeAttributes = SetWindowLongA(window_handle, -20, (LONG_PTR)(getInfo | 0x20));

	// avoiding window flags: https://www.unknowncheats.me/forum/general-programming-and-reversing/229070-overlay-window-topmost-top-game-windows-video.html
	MARGINS margins = { -1 };
	margins.cyBottomHeight = margins.cyTopHeight = margins.cxLeftWidth = margins.cxRightWidth = -1;
	DwmExtendFrameIntoClientArea(window_handle, &margins);
	SetLayeredWindowAttributes(window_handle, 0x000000, 0xFF, 0x02);
	SetWindowPos(window_handle, HWND_TOPMOST, 0, 0, 0, 0, 0x0002 | 0x0001);

	if (!window_handle)
		return; // error grabbing nvidia overlay

	font = new s_font();

	if (!init_device())
		return; // error creating device

	init_imgui();

	std::thread(&c_overlay::input_handler, this).detach();
	std::thread(&c_overlay::anim_handler, this).detach();
}

c_overlay::~c_overlay() {
	static const ImVec4 clear_clr = ImVec4{ 0, 0, 0, 0 };
	device_context->ClearRenderTargetView(render_target_view, (float*)&clear_clr);

	dest_imgui();
	dest_device();

	delete font;
}


void c_overlay::box_with_glow(const ImVec2 & min, const ImVec2 & max, ImU32 color, FLOAT line_width, FLOAT glow_size) {
	ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

	
	float alpha = (color >> 24 & 0xFF) / 255.0f;
	ImVec4 color_vec = ImGui::ColorConvertU32ToFloat4(color);

	
	float glow_intensity = 0.3f; 

	
	for (int i = 1; i <= glow_size; i++) {
		
		float glow_alpha = alpha * glow_intensity * (1.0f - (float)i / glow_size);
		ImU32 glow_color = ImGui::ColorConvertFloat4ToU32(ImVec4(color_vec.x, color_vec.y, color_vec.z, glow_alpha));

		
		draw_list->AddRect(
			ImVec2(min.x - i, min.y - i),
			ImVec2(max.x + i, max.y + i),
			glow_color,
			0.0f,
			0,
			line_width
		);

		
		draw_list->AddRect(
			ImVec2(min.x + i, min.y + i),
			ImVec2(max.x - i, max.y - i),
			glow_color,
			0.0f, 
			0,
			line_width
		);
	}

	
	draw_list->AddRect(min, max, color, 0.0f, 0, line_width);
}


const void c_overlay::renderCheats()
{
	printFPS;

	if (config.activeCheats.esp)
	{
		const auto& enemyColor = selectedColorIt->second;
		const auto& friendlyColor = rgbColors::black;

		for (const auto& espRenderData : espData.boxes)
		{
			if (!espRenderData.isValid)
				continue;

			if (config.esp.boxGlow)
			{
				float dynamicGlowSize = 5.0f + sin((float)ImGui::GetTime() * 2.0f) * 3.0f;

				if (!espRenderData.isFriendly)
					box_with_glow(espRenderData.min, espRenderData.max, enemyColor, 2.0f, dynamicGlowSize);
				else if (config.esp.renderFriendlies)
					box_with_glow(espRenderData.min, espRenderData.max, friendlyColor, 2.0f, dynamicGlowSize);
			}
			else
			{
				if (!espRenderData.isFriendly)
					box(espRenderData.min, espRenderData.max, enemyColor);
				else if (config.esp.renderFriendlies)
					box(espRenderData.min, espRenderData.max, friendlyColor);
			}

			const auto name_text = espRenderData.playerName;
			const auto text_size = ImGui::CalcTextSize(name_text);

			const auto text_pos = ImVec2{
				(espRenderData.max.x + espRenderData.min.x) / 2.f - (text_size.x / 2.f), espRenderData.max.y
			};

			text(text_pos, name_text, enemyColor);
		}
	}

	if (config.activeCheats.aimBot)
	{
		circle(ImVec2(window_width / 2.f, window_height / 2.f), config.aimBot.pixelRadius, rgbColors::transparent_white);
	}
}


void c_overlay::draw_menu()
{
	ImGui::SetCurrentContext(menuContext);

	static bool isFirstIteration = true;

	if (isFirstIteration)
	{
		// Main window position
		ImGui::SetNextWindowPos(persistentConfig.mainWindow.windowPosition);
		isFirstIteration = false;
	}

	ImGui::SetNextWindowSize(ImVec2(500, 500));

	// imgui menu itself
	ImGui::Begin(
		"Main GUI",
		0,
		ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoSavedSettings
	);

	persistentConfig.mainWindow.windowPosition = ImGui::GetWindowPos();

	ImGui::Text("Merkur external");
	if (ImGui::BeginTabBar("MyTabBar")) {
		if (ImGui::BeginTabItem("aim")) {
			ImGui::Checkbox("triggerbot", &config.activeCheats.triggerBot);
			ImGui::Checkbox("aimbot", &config.activeCheats.aimBot);
			if (config.activeCheats.aimBot == true)
			{
				(ImGui::SliderFloat("Fov",  &config.aimBot.pixelRadius, 0, 400));
				(ImGui::SliderFloat("Smooth", &config.aimBot.speedFactor, 0, 2));
			}
			ImGui::EndTabItem();
		}
		
		if (ImGui::BeginTabItem("visuals")) {
			ImGui::BeginChild("##container", ImVec2(190, 275), false, ImGuiWindowFlags_NoScrollbar); {
				
				ImGui::Checkbox("esp", &config.activeCheats.esp);
				if (config.activeCheats.esp == true)
				{
					if (ImGui::BeginCombo("Color", selectedColorIt->first.data())) {
						// Draw all colors names to combobox
						for (auto it = availableColors.begin(); it != availableColors.end(); ++it)
						{
							const auto& [colorName, color] = *it;

							bool isSelected = (selectedColorIt->first == colorName);

							if (ImGui::Selectable(colorName.data(), isSelected))
							{
								// Update selection if something was selected
								memset(config.esp.selectedColor, 0, COLOR_NAME_MAX_LENGTH);
								colorName.copy(config.esp.selectedColor, min(colorName.length(), COLOR_NAME_MAX_LENGTH));
								config.esp.selectedColor[COLOR_NAME_MAX_LENGTH - 1] = '\0';

								selectedColorIt = it;
							}
							if (isSelected)
								ImGui::SetItemDefaultFocus(); // Keep the selected item focused
						}
						ImGui::EndCombo();
					}
					ImGui::Checkbox("box glow", &config.esp.boxGlow);
				}
			}
			ImGui::EndChild();


			ImGui::SetCursorPos({ 285, 95 });
			ImGui::BeginChild("##container1", ImVec2(190, 275), false, ImGuiWindowFlags_NoScrollbar); {
				ImGui::Checkbox("radar", &config.activeCheats.radar);
				ImGui::Checkbox("antiflash", &config.activeCheats.antiFlash);
			}
			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		
		if (ImGui::BeginTabItem("misc")) {
			ImGui::Checkbox("Fov changer", &config.activeCheats.fovChanger);
			if (config.activeCheats.fovChanger == true)
			{
				(ImGui::SliderInt("Fov", &config.fov.desiredFov, 60, 140));
			}
			ImGui::Checkbox("bomb info", &config.activeCheats.bombHack);
			ImGui::Checkbox("soon", &l_test);
			ImGui::EndTabItem();
		}
		
		if (ImGui::BeginTabItem("config")) {
			
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
}


void c_overlay::draw_bomb()
{
	static bool isFirstIteration = true;

	if (isFirstIteration)
	{
		// Main window position
		ImGui::SetNextWindowPos(persistentConfig.bombWindow.windowPosition);
		isFirstIteration = false;
	}

	ImVec4 outlineColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	ImGui::SetNextWindowSize(ImVec2(300, 100));
	ImGui::Begin(
		"Bomb GUI",
		0,
		ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoBackground
	);

	persistentConfig.bombWindow.windowPosition = ImGui::GetWindowPos();

	if (bombData.planted == true)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
		ImGui::Text("Bomb has been planted.");
		if (bombData.timeRemaining > 0)
		{
			ImGui::Text("Time remaining: %.2f", bombData.timeRemaining);
		}
		else
		{
			ImGui::Text("If I was you i would RUN!!");
		}
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
		ImGui::Text("Bomb is not planted.");

	}
	ImGui::PopStyleColor();
	ImGui::End();
}


const void c_overlay::render(FLOAT width, FLOAT height) {
	if (exit) {
		init_draw_list();
		ready_to_close = true;
		return;
	}

	init_draw_list(); // we only do this once a loop

//#define pos_sin(x) ((sin(x) + 1.) / 2.)
//	const auto time = ImGui::GetTime();
//	const auto col1 = IM_COL32(255.f * pos_sin(time + 0.f), 255.f * pos_sin(time + 1.f), 255.f * pos_sin(time + 2.f), 64.f);
//	const auto col2 = IM_COL32(255.f * pos_sin(time + 1.f), 255.f * pos_sin(time + 2.f), 255.f * pos_sin(time + 3.f), 64.f);
//	radial_gradient(ImVec2(window_width / 2.f, window_height / 2.f), 400, col2, col1);

	std::string name = "Merkur Cheats";
	text({ window_width - ImGui::CalcTextSize(name.c_str()).x - 5, 0 }, name, rgbColors::off_white, true);

	renderCheats();

	if (config.activeCheats.bombHack)
		draw_bomb();

	if (!show_menu)
		return;

	draw_menu();
}

void c_overlay::initConfigVariables()
{
	for (auto it = availableColors.begin(); it != availableColors.end(); ++it)
	{
		const auto& [colorName, color] = *it;

		if (std::string_view(config.esp.selectedColor) == colorName)
		{
			selectedColorIt = it;
			break;
		}
	}
}

BOOL c_overlay::msg_loop() {
	if (window_handle) {
		MSG msg{ 0 };
		ImVec4 clear_clr = { 0,0,0,0 };

		if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		render(window_width, window_height);

		ImGui::Render();
		device_context->OMSetRenderTargets(1, &render_target_view, NULL);
		device_context->ClearRenderTargetView(render_target_view, (float*)&clear_clr); // might break, if so then put cheat color of type ImVec4 { 0,0,0,0 }
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		swap_chain->Present(0, 0);

		return msg.message != WM_QUIT;
	}
	return false;
}

void c_overlay::input_handler() {
	// added a sleep per user @colbyfortnite's suggestion
	for (; !exit; Sleep(1)) {
		// for our imgui menu interaction
		ImGuiIO& io = ImGui::GetIO();

		POINT p{};
		GetCursorPos(&p);
		io.MousePos = ImVec2((float)p.x, (float)p.y);

		io.MouseDown[0] = GetAsyncKeyState(VK_LBUTTON) & 0x8000;
		io.MouseDown[1] = GetAsyncKeyState(VK_RBUTTON) & 0x8000;
	}
}

void c_overlay::anim_handler() {
	// timers for our animations
	breath_timer.start();

	bool menuKeyReleased = true;

	for (;!exit;) {
		RECT rect{ 0 };
		POINT point{ 0 };
		GetClientRect(window_handle, &rect);
		SetWindowPos(window_handle, HWND_TOPMOST, rect.left, rect.top, rect.right, rect.bottom, SWP_NOREDRAW);

		// panic key
		exit = GetAsyncKeyState(keybinds::quickExit) & 0x8000;

		// timer for our breathing animation
		int breath_tmr = breath_timer.end();
		if (breath_tmr >= breath_anim_time * 2) {
			breath = breath_anim_time;
			breath_timer.start();
		}
		else if (breath_tmr > breath_anim_time)
			breath = breath_anim_time - (breath_tmr - breath_anim_time);
		else if (breath_tmr <= breath_anim_time)
			breath = breath_tmr;

		// Show/Hide menu
		if (GetAsyncKeyState(keybinds::showMenu) & 0x8000) {
			if (menuKeyReleased) 
				show_menu = !show_menu;

			menuKeyReleased = false;
		} else
			menuKeyReleased = true;
	}
}

VOID c_overlay::init_draw_list() {
	draw_list = ImGui::GetBackgroundDrawList();
}

//to make esp box
VOID c_overlay::box(const ImVec2& min, const ImVec2& max, ImU32 color, const FLOAT line_width) {
	// The anti-aliasing for the box is handled through the rasterizer state in DirectX.
	draw_list->AddRect(min, max, color, 0.0f, 0, line_width);
}


//esp tracer
VOID c_overlay::line(const ImVec2& point1, const ImVec2 point2, ImU32 color, const FLOAT line_width) {
	draw_list->AddLine(point1, point2, color, line_width);
}
//aimbot fov circle
VOID c_overlay::circle(const ImVec2& point, const FLOAT radius, ImU32 color) {
	draw_list->AddCircle(point, radius, color, 200, 3.f); // num segments is how circular the circle actually is
}
//esp names
VOID c_overlay::text(const ImVec2& pos, const std::string& text, ImU32 color, bool outline) {
	if (outline) {
		draw_list->AddText(font->im_font, font->im_font->FontSize, { pos.x + 1, pos.y }, IM_COL32_BLACK, text.c_str());
		draw_list->AddText(font->im_font, font->im_font->FontSize, { pos.x - 1, pos.y }, IM_COL32_BLACK, text.c_str());
		draw_list->AddText(font->im_font, font->im_font->FontSize, { pos.x, pos.y + 1 }, IM_COL32_BLACK, text.c_str());
		draw_list->AddText(font->im_font, font->im_font->FontSize, { pos.x, pos.y - 1 }, IM_COL32_BLACK, text.c_str());
	}
	draw_list->AddText(font->im_font, font->im_font->FontSize, pos, color, text.c_str());
}
// Filled circle. creates a gradient from middle color to outer color
VOID c_overlay::radial_gradient(const ImVec2& center, float radius, ImU32 col_in, ImU32 col_out) {
	if (((col_in | col_out) & IM_COL32_A_MASK) == 0 || radius < 0.5f)
		return;

	draw_list->_PathArcToFastEx(center, radius, 0, IM_DRAWLIST_ARCFAST_SAMPLE_MAX, 0);
	const int count = draw_list->_Path.Size - 1;

	unsigned int vtx_base = draw_list->_VtxCurrentIdx;
	draw_list->PrimReserve(count * 3, count + 1);

	const ImVec2 uv = draw_list->_Data->TexUvWhitePixel;
	draw_list->PrimWriteVtx(center, uv, col_in);
	for (int n = 0; n < count; n++)
		draw_list->PrimWriteVtx(draw_list->_Path[n], uv, col_out);

	for (int n = 0; n < count; n++) {
		draw_list->PrimWriteIdx((ImDrawIdx)(vtx_base));
		draw_list->PrimWriteIdx((ImDrawIdx)(vtx_base + 1 + n));
		draw_list->PrimWriteIdx((ImDrawIdx)(vtx_base + 1 + ((n + 1) % count)));
	}
	draw_list->_Path.Size = 0;
}

BOOL c_overlay::in_screen(const ImVec2& pos) {
	return !(pos.x > window_width || pos.x<0 || pos.y>window_height || pos.y < 0);
}