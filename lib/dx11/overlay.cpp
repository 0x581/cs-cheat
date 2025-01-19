#include "../pch.h"
#include "../../shared/sharedVariables.hpp"
#include "../../shared/defines.hpp"
#include "../../shared/colors.hpp"


// microsoft safe release of pointers
template <typename T>
inline VOID c_overlay::safe_release(T*& p) {
	if (NULL != p) {
		p->Release();
		p = NULL;
	}
}
int sliderValue = 100; // ignore (fck nicolas das bleib da)
static bool triggerbot = activeCheats.triggerBot.load();
static bool radar = activeCheats.radar.load();
static bool antiflash = activeCheats.antiFlash.load();
static bool esp = activeCheats.esp.load();
static bool fov = activeCheats.fovChanger.load();
static bool bomb = activeCheats.bombHack.load();
static bool l_test = false;
static ImU32 selectedColor = rgbColors::green;  // Default selected color

// Dropdown menu to select color
const char* colorNames[] = { "Green", "Red", "Blue", "Yellow", "Black", "White" };
ImU32 colors[] = { rgbColors::green, rgbColors::red, rgbColors::blue, rgbColors::yellow, rgbColors::black, rgbColors::white };


ImGuiContext* context1 = ImGui::CreateContext();
BOOL c_overlay::init_device() {
	DXGI_SWAP_CHAIN_DESC vSwapChainDesc;
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
	vSwapChainDesc.SampleDesc.Count = 1;
	vSwapChainDesc.SampleDesc.Quality = 0;
	vSwapChainDesc.Windowed = TRUE;
	vSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	D3D_FEATURE_LEVEL vFeatureLevel;
	D3D_FEATURE_LEVEL vFeatureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, vFeatureLevelArray, 2, D3D11_SDK_VERSION, &vSwapChainDesc, &swap_chain, &d3d_device, &vFeatureLevel, &device_context) != S_OK)
		return false;

	init_render_target();

	ShowWindow(window_handle, SW_SHOWNORMAL);
	UpdateWindow(window_handle);
	return true;
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




	ImGui::SetCurrentContext(context1);
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
	ready(false),
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


const void c_overlay::renderCheats() {
	for (size_t i = 0; i < 64; i++)
	{
		auto pair_ptr = espData.boxes[i];

		if (!pair_ptr)
			continue;

		box(pair_ptr->first, pair_ptr->second, selectedColor);
	}
}

void c_overlay::menu_main() {


	ImGui::SetCurrentContext(context1);
	ImGui::SetNextWindowSize(ImVec2(500, 500));


	// imgui menu itself
	ImGui::Begin(
		"Misc",
		0,
		ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoSavedSettings
	);

	ImGui::Text("0x581 external");
	if (ImGui::BeginTabBar("MyTabBar")) {
		if (ImGui::BeginTabItem("aim")) {
			ImGui::Checkbox("triggerbot", &triggerbot);
			
			ImGui::Checkbox("soon", &l_test);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("visuals")) {
			ImGui::Checkbox("radar", &radar);
			ImGui::Checkbox("antiflash", &antiflash);
			ImGui::Checkbox("esp", &esp);
			if(esp == true)
			{ 
				if (ImGui::BeginCombo("ESP Color", colorNames[0])) {
					for (int i = 0; i < IM_ARRAYSIZE(colorNames); i++) {
						bool isSelected = (selectedColor == colors[i]);
						if (ImGui::Selectable(colorNames[i], isSelected)) {
							selectedColor = colors[i];
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus(); // Keep the selected item focused
					}
					ImGui::EndCombo();
				}
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("misc")) {
			ImGui::Checkbox("Fov changer", &fov);
			if (fov == true)
			{
				(ImGui::SliderInt("Fov", &sliderValue, 60, 140));
			}
			ImGui::Checkbox("bomb info", &bomb);
			ImGui::Checkbox("soon", &l_test);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
}

const void c_overlay::render(FLOAT width, FLOAT height) {
	if (exit) {
		init_draw_list();
		ready = true;
		return;
	}



	init_draw_list(); // we only do this once a loop

	renderCheats();


	std::string name = "0x581 external"; // Beispiel für eine Versionsnummer
	text({ window_width - ImGui::CalcTextSize(name.c_str()).x, 0 }, name);



	if (bomb == true)
	{
		ImVec4 outlineColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		ImGui::SetNextWindowSize(ImVec2(200, 200));
		ImGui::Begin(
			"lol",
			0,
			ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoBackground
		);

		if (bombData.planted == true)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
			ImGui::Text("bomb is planted");
			ImGui::Text("Time remaining:", bombData.timeRemaining);
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
			ImGui::Text("is not planted");

		}
		ImGui::PopStyleColor();
		ImGui::End();
	}

	if (!show_menu)
		return;

	activeCheats.triggerBot.store(triggerbot);
	activeCheats.radar.store(radar);
	activeCheats.antiFlash.store(antiflash);
	activeCheats.esp.store(esp);
	activeCheats.fovChanger.store(fov);

	fovData.desiredFov = sliderValue;


	menu_main();


	/*ImGui::SetNextWindowSize(ImVec2(1000, 1000));
	ImGui::Begin(
		"ll",
		0,
		ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoSavedSettings

	);

	ImGui::ShowStyleEditor();

	ImGui::End();
	*/

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
		swap_chain->Present(1, 0);

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
	draw_list->AddRect(min, max, color, 0, 0, line_width);
}

//esp tracer
VOID c_overlay::line(const ImVec2& point1, const ImVec2 point2, ImU32 color, const FLOAT line_width) {
	draw_list->AddLine(point1, point2, color, line_width);
}
//aimbot fov circle
VOID c_overlay::circle(const ImVec2& point, const FLOAT radius, ImU32 color) {
	draw_list->AddCircleFilled(point, radius, color, 200); // num segments is how circular the circle actually is
}
//esp names
VOID c_overlay::text(const ImVec2& pos, const std::string& text, ImU32 color, bool outline) {
	if (outline) {
		draw_list->AddText(font->im_font, 18.f, { pos.x + 1, pos.y }, IM_COL32_BLACK, text.c_str());
		draw_list->AddText(font->im_font, 18.f, { pos.x - 1, pos.y }, IM_COL32_BLACK, text.c_str());
		draw_list->AddText(font->im_font, 18.f, { pos.x, pos.y + 1 }, IM_COL32_BLACK, text.c_str());
		draw_list->AddText(font->im_font, 18.f, { pos.x, pos.y - 1 }, IM_COL32_BLACK, text.c_str());
	}
	draw_list->AddText(font->im_font, 18.f, pos, color, text.c_str());
}
//?
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