#include "../overlay.h"
#include "../../lib/uiaccess/uiaccess.h"

#include <windows.h>


#include "shared_variables.h"
#include "colors.h"
#include "config.h"

#include "keybinds.h"

#include <string>
#include <vector>
#include <map>

using std::string, std::vector, std::map;

static int current_main_nav_item = 0;
const char* nav_items[] = { "Aim", "Visuals", "Misc", "Config" };


vector<string> configs = { "Default", "Legit", "Rage" };  // Beispiel-Configs
char configNameBuffer[64] = "";  // Eingabefeld für neue Config-Namen
int selectedConfig = -1;  // Index der aktuell ausgewählten Config

// microsoft safe release of pointers
template <typename T>
inline VOID c_overlay::safe_release(T*& p) {
	if (NULL != p)
	{
		p->Release();
		p = NULL;
	}
}


ImGuiContext* menuContext = ImGui::CreateContext();


BOOL use_uiaccess(HWND* pHandle) {

	typedef HWND(WINAPI* CreateWindowInBand)(
		DWORD dwExStyle,
		LPCWSTR lpClassName,
		LPCWSTR lpWindowName,
		DWORD dwStyle,
		int x,
		int y,
		int nWidth,
		int nHeight,
		HWND hWndParent,
		HMENU hMenu,
		HINSTANCE hInstance,
		LPVOID lpParam,
		DWORD dwBand
		);


	DWORD dwErr = PrepareForUIAccess();

	if (dwErr != ERROR_SUCCESS)
		return dwErr;

	constexpr auto ZBID_UIACCESS = 2; // UIAccess Band ID

	const auto user32_dll = LoadLibrary(L"user32.dll");
	const auto pCreateWindowInBand = (CreateWindowInBand)GetProcAddress(user32_dll, "CreateWindowInBand");

	//WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"ImGui Example", NULL };
	//auto res = RegisterClassExW(&wc);

	//HWND hwnd = pCreateWindowInBand(WS_EX_TOPMOST | WS_EX_NOACTIVATE, res, NULL, 0x80000000, 0, 0, 0, 0, NULL, NULL, wndParentClass.hInstance, LPVOID(res), ZBID_UIACCESS);
}


BOOL hook_nvidia(HWND* pHandle) {
	HWND handle = FindWindow(L"CEF-OSC-WIDGET", L"NVIDIA GeForce Overlay");

	if (!handle)
	{
		std::cout << "Failed to find NVIDIA Overlay window." << std::endl;
		return false;
	}

	auto style = GetWindowLongPtr(handle, GWL_EXSTYLE);
	style |= WS_EX_TRANSPARENT; // | WS_EX_LAYERED;
	SetWindowLongPtr(handle, GWL_EXSTYLE, style);

	// avoiding window flags: https://www.unknowncheats.me/forum/general-programming-and-reversing/229070-overlay-window-topmost-top-game-windows-video.html
	MARGINS margins = {
		.cxLeftWidth = -1,
		.cxRightWidth = -1,
		.cyTopHeight = -1,
		.cyBottomHeight = -1,
	};

	DwmExtendFrameIntoClientArea(handle, &margins);
	SetLayeredWindowAttributes(handle, 0, 0xFF, LWA_ALPHA);
	SetWindowPos(handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	*pHandle = handle;

	return true;
}


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

VOID c_overlay::dest_device() {
	dest_render_target();
	safe_release(swap_chain);
	safe_release(device_context);
	safe_release(d3d_device);
}

VOID c_overlay::init_imgui() {
	IMGUI_CHECKVERSION();
	ImGui::SetCurrentContext(menuContext); // Wichtig: Kontext setzen, bevor Style geändert wird

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
	ImGuiIO& io = ImGui::GetIO();
	if (font)
	{
		font->im_font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 16.0f);
		font->font_size = 16.f;
	}
	else
	{
		io.Fonts->AddFontDefault();
	}

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
	breath(NULL) {

	if (!use_uiaccess(&window_handle))
		throw std::runtime_error(string("Failed to get UI access. Error: ") + std::to_string(GetLastError()));

	//if (!hook_nvidia(&window_handle))
	//	throw std::runtime_error(string("Failed to hook NVIDIA overlay."));

	font = new s_font();

	if (!init_device())
		throw std::runtime_error(string("Failed to initialize DX device."));

	init_imgui();

	std::thread(&c_overlay::input_handler, this).detach();
	std::thread(&c_overlay::anim_handler, this).detach();
}

c_overlay::~c_overlay() {
	clear_screen();

	dest_imgui();
	dest_device();

	delete font;
}


const void c_overlay::renderCheats() {
	if (config.activeCheats.esp)
	{
		float dynamicGlowSize = 0.0f;
		if (config.esp.boxGlow)
		{ // Nur berechnen, wenn Glow überhaupt aktiv sein könnte
// Die Berechnung selbst kann bleiben, aber sie geschieht jetzt außerhalb der Schleife
			dynamicGlowSize = 5.0f + sin((float)ImGui::GetTime() * 2.0f) * 3.0f;
		}

		const ImU32 enemyColorU32 = ImGui::ColorConvertFloat4ToU32(config.esp.colorEnemyVisible);
		const ImU32 teamColorU32 = ImGui::ColorConvertFloat4ToU32(config.esp.colorTeamVisible);

		for (const auto& espRenderData : espData.data)
		{
			if (!espRenderData.isValid)
				continue;

			ImU32 currentColorU32;
			bool shouldRenderThisEntity = false;

			if (!espRenderData.isFriendly)
			{
				currentColorU32 = enemyColorU32;
				shouldRenderThisEntity = true;
			}
			else
			{
				if (config.esp.renderFriendlies)
				{
					currentColorU32 = teamColorU32;
					shouldRenderThisEntity = true;
				}
				else
				{
					continue;
				}
			}

			if (!shouldRenderThisEntity) continue;

			const auto min = espRenderData.min;
			const auto max = espRenderData.max;
			if (!((min.x >= 0.f && min.y >= 0.f && min.x < window_width && min.y < window_height) ||
				(max.x >= 0.f && max.y >= 0.f && max.x < window_width && max.y < window_height)))
				continue;

			// boneesp ESP
			if (config.esp.bone)
			{
				const BoneData& bones = espRenderData.bones;
				if (!espRenderData.isFriendly || config.esp.renderFriendlies)
				{
					line(bones.head, bones.neck, currentColorU32);
					line(bones.neck, bones.spine4, currentColorU32);
					line(bones.spine4, bones.spine3, currentColorU32);
					line(bones.spine3, bones.spine2, currentColorU32);
					line(bones.spine2, bones.spine1, currentColorU32);
					line(bones.spine1, bones.hip, currentColorU32);
					line(bones.spine4, bones.l_shoulder, currentColorU32);
					line(bones.l_shoulder, bones.l_arm, currentColorU32);
					line(bones.l_arm, bones.l_wrist, currentColorU32);
					line(bones.l_wrist, bones.l_hand, currentColorU32);
					line(bones.spine4, bones.r_shoulder, currentColorU32);
					line(bones.r_shoulder, bones.r_arm, currentColorU32);
					line(bones.r_arm, bones.r_wrist, currentColorU32);
					line(bones.r_wrist, bones.r_hand, currentColorU32);
					line(bones.hip, bones.l_hip, currentColorU32);
					line(bones.l_hip, bones.l_knee, currentColorU32);
					line(bones.l_knee, bones.l_foot, currentColorU32);
					line(bones.hip, bones.r_hip, currentColorU32);
					line(bones.r_hip, bones.r_knee, currentColorU32);
					line(bones.r_knee, bones.r_foot, currentColorU32);
				}
			}

			// Box ESP
			if (config.esp.boxesp)
			{
				if (config.esp.boxGlow)
				{
					// Verwende die vorab berechnete dynamicGlowSize
					box_with_glow(min, max, currentColorU32, 2.0f, dynamicGlowSize);
				}
				else
				{
					box(min, max, currentColorU32);
				}
			}

			if (config.esp.nameesp == true)
			{
				if (!espRenderData.isFriendly || config.esp.renderFriendlies)
				{
					const auto name_text = espRenderData.playerName;
					// const auto text_size = ImGui::CalcTextSize(name_text); 
					const auto text_pos = ImVec2{ (max.x + min.x) / 2.f - (ImGui::CalcTextSize(name_text).x / 2.f), max.y };
					text(text_pos, name_text, currentColorU32);
				}
			}
		}
	}

	if (config.activeCheats.aimBot)
	{
		if (config.aimBot.drawFovCircle)
		{
			ImU32 circleColorU32 = ImGui::ColorConvertFloat4ToU32(config.aimBot.fovCircleColor);
			circle(ImVec2(window_width / 2.f, window_height / 2.f), config.aimBot.pixelRadius, circleColorU32);
		}
	}
}


void c_overlay::draw_menu() {
	ImGui::SetCurrentContext(menuContext);
	ImGuiStyle& style = ImGui::GetStyle();

	static bool isFirstIteration = true;
	if (isFirstIteration)
	{
		ImGui::SetNextWindowPos(persistentConfig.mainWindow.windowPosition);
		isFirstIteration = false;
	}

	ImGui::SetNextWindowSize(ImVec2(650, 500), ImGuiCond_FirstUseEver);

	ImGui::Begin("Main GUI",
		nullptr,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings);

	persistentConfig.mainWindow.windowPosition = ImGui::GetWindowPos();

	float nav_bar_width = 130.0f;
	ImGui::BeginChild("NavBar", ImVec2(nav_bar_width, 0), true, ImGuiWindowFlags_NoScrollbar);
	{
		ImGui::PushFont(font->im_font);
		ImGui::Text("Merkur");
		ImGui::PopFont();
		ImGui::Separator();
		ImGui::Spacing();
		for (int i = 0; i < IM_ARRAYSIZE(nav_items); ++i)
		{
			bool is_selected = (current_main_nav_item == i);
			if (is_selected)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_TabActive]);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, style.Colors[ImGuiCol_TabActive]);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, style.Colors[ImGuiCol_TabActive]);
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_Tab]);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, style.Colors[ImGuiCol_TabHovered]);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, style.Colors[ImGuiCol_TabActive]);
			}
			if (ImGui::Button(nav_items[i], ImVec2(nav_bar_width - style.WindowPadding.x * 2, 30)))
			{
				current_main_nav_item = i;
			}
			ImGui::PopStyleColor(3);
			ImGui::Spacing();
		}
	}
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("ContentArea", ImVec2(0, 0), true);
	{
		ImGui::Text("%s Settings", nav_items[current_main_nav_item]);
		ImGui::Separator();
		ImGui::Spacing();

		float label_width = 80.0f;

		switch (current_main_nav_item)
		{
		case 0: // Aim
			if (ImGui::CollapsingHeader("Aimbot Options", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Checkbox("Triggerbot", &config.activeCheats.triggerBot);
				ImGui::Checkbox("Enable Aimbot", &config.activeCheats.aimBot);
				if (config.activeCheats.aimBot)
				{
					ImGui::Indent();

					ImGui::Checkbox("Draw FOV Circle", &config.aimBot.drawFovCircle);
					if (config.aimBot.drawFovCircle)
					{
						ImGui::SameLine(0, style.ItemSpacing.x * 2);
						ImGui::ColorEdit4("##FovCircleColor", (float*)&config.aimBot.fovCircleColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
						ImGui::Text("FOV");
						ImGui::SameLine(label_width + style.ItemInnerSpacing.x);
						ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - style.ItemInnerSpacing.x);
						ImGui::SliderFloat("##FOV", &config.aimBot.pixelRadius, 0.0f, 400.0f, "%.0fpx");
						ImGui::PopItemWidth();
					}
					ImGui::Text("Smooth");
					ImGui::SameLine(label_width + style.ItemInnerSpacing.x);
					ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - style.ItemInnerSpacing.x);
					ImGui::SliderFloat("##Smooth", &config.aimBot.speedFactor, 0.0f, 2.0f, "%.2f");
					ImGui::PopItemWidth();
					ImGui::Checkbox("Ignore Team", &config.aimBot.ignoreteam);
					ImGui::Checkbox("visible(soon)", &config.aimBot.visible);


					ImGui::Unindent();
				}
			}
			break;

		case 1: // Visuals
			if (ImGui::CollapsingHeader("ESP General", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Checkbox("Enable ESP", &config.activeCheats.esp);
				if (config.activeCheats.esp)
				{
					ImGui::Indent();
					ImGui::Checkbox("Show Teammates", &config.esp.renderFriendlies);
					ImGui::Checkbox("Bone ESP", &config.esp.bone);
					ImGui::Checkbox("Name ESP", &config.esp.nameesp);
					ImGui::Checkbox("Box ESP", &config.esp.boxesp);
					if (config.esp.boxesp)
					{
						ImGui::Indent();
						ImGui::Checkbox("Box Glow", &config.esp.boxGlow);
						ImGui::Unindent();
					}
					ImGui::Unindent();
				}
			}
			ImGui::Separator();

			if (config.activeCheats.esp)
			{
				if (ImGui::CollapsingHeader("ESP Colors", ImGuiTreeNodeFlags_DefaultOpen))
				{

					float color_label_width = 180.0f;
					ImGui::Text("Enemy Visible");
					ImGui::SameLine(color_label_width);
					ImGui::ColorEdit4("##EnemyVisColor", (float*)&config.esp.colorEnemyVisible, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

					ImGui::Text("Enemy Hidden(soon)");
					ImGui::SameLine(color_label_width);
					ImGui::ColorEdit4("##EnemyHidColor", (float*)&config.esp.colorEnemyHidden, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

					if (config.esp.renderFriendlies)
					{
						ImGui::Text("Team Visible");
						ImGui::SameLine(color_label_width);
						ImGui::ColorEdit4("##TeamVisColor", (float*)&config.esp.colorTeamVisible, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

						ImGui::Text("Team Hidden(soon)");
						ImGui::SameLine(color_label_width);
						ImGui::ColorEdit4("##TeamHidColor", (float*)&config.esp.colorTeamHidden, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
					}
				}
			}
			ImGui::Separator();
			if (ImGui::CollapsingHeader("Other Visuals", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Checkbox("Radar(risky)", &config.activeCheats.radar);
				ImGui::Checkbox("Anti-Flash(risky)", &config.activeCheats.antiFlash);
			}
			break;

		case 2: // Misc
			if (ImGui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Checkbox("FOV Changer", &config.activeCheats.fovChanger);
				if (config.activeCheats.fovChanger)
				{
					ImGui::Indent();
					ImGui::Text("FOV");
					ImGui::SameLine(label_width + style.ItemInnerSpacing.x);
					ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - style.ItemInnerSpacing.x);
					ImGui::SliderInt("##FOVValue", &config.fov.desiredFov, 60, 140);
					ImGui::PopItemWidth();
					ImGui::Unindent();
				}
				ImGui::Checkbox("Bomb Info Panel™", &config.activeCheats.bombHack);
			}
			break;

		case 3: // Config
			if (ImGui::CollapsingHeader("Manage Configurations", ImGuiTreeNodeFlags_DefaultOpen))
			{
				float halfWidth = ImGui::GetContentRegionAvail().x * 0.48f;
				ImGui::BeginChild("ConfigListPane", ImVec2(halfWidth, 150), true);
				ImGui::Text("Saved Configs:");
				if (ImGui::ListBoxHeader("##ConfigList", ImVec2(-1.0f, -1.0f)))
				{
					for (size_t i = 0; i < configs.size(); ++i)
					{
						const bool is_selected = (static_cast<int>(i) == selectedConfig);
						if (ImGui::Selectable(configs[i].c_str(), is_selected))
						{
							selectedConfig = static_cast<int>(i);
							strncpy_s(configNameBuffer, sizeof(configNameBuffer), configs[i].c_str(), _TRUNCATE);
						}
						if (is_selected) ImGui::SetItemDefaultFocus();
					}
					ImGui::ListBoxFooter();
				}
				ImGui::EndChild();
				ImGui::SameLine();
				ImGui::BeginChild("ConfigActionsPane", ImVec2(0, 150), true);
				ImGui::InputText("Name", configNameBuffer, IM_ARRAYSIZE(configNameBuffer));
				if (ImGui::Button("Save", ImVec2(ImGui::GetContentRegionAvail().x, 0))) { /*Logik*/ }
				if (ImGui::Button("Load", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
				{
					if (selectedConfig != -1) initConfigVariables(); /*Logik*/
				}
				if (ImGui::Button("Delete", ImVec2(ImGui::GetContentRegionAvail().x, 0))) { /*Logik*/ }
				ImGui::EndChild();
			}
			break;
		}
	}
	ImGui::EndChild();
	ImGui::End();
}

void c_overlay::draw_bomb() {
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
	if (exit)
	{
		//init_draw_list();
		//clear_screen();
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

	if (config.activeCheats.bombHack)
		draw_bomb();

	if (!show_menu)
		return;

	draw_menu();
}


BOOL c_overlay::msg_loop() {
	if (window_handle)
	{
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

void c_overlay::clear_screen() {
	constexpr ImVec4 clear_clr = { 0,0,0,0 };

	device_context->ClearRenderTargetView(render_target_view, (float*)&clear_clr);
}

void c_overlay::input_handler() {
	// added a sleep per user @colbyfortnite's suggestion
	for (; !exit; Sleep(1))
	{
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

	for (;!exit;)
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

VOID c_overlay::init_draw_list() {
	draw_list = ImGui::GetBackgroundDrawList();
}

BOOL c_overlay::in_screen(const ImVec2& pos) {
	return !(pos.x > window_width || pos.x<0 || pos.y>window_height || pos.y < 0);
}
void c_overlay::initConfigVariables()
{
	//nothing
}