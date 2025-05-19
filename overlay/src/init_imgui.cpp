#include "../overlay.h"

#include <imgui.h>

#include <config.h>

#include "../window_handle.h"

static void set_style_colors()
{
	auto colors = ImGui::GetStyle().Colors;

	// --- Theme Colors with Purple Accent ---
	constexpr ImVec4 almostBlack = ImVec4(0.06f, 0.06f, 0.07f, 0.95f);
	constexpr ImVec4 darkGray = ImVec4(0.10f, 0.10f, 0.12f, 0.95f);
	// Lila Akzentfarben
	constexpr ImVec4 brightPurple = ImVec4(0.5f, 0.2f, 0.8f, 1.00f);      // Ein kräftiges Lila
	constexpr ImVec4 brightPurpleHover = ImVec4(0.6f, 0.3f, 0.9f, 1.00f); // Etwas helleres Lila für Hover/Aktiv
	constexpr ImVec4 lightText = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
	constexpr ImVec4 darkOutline = ImVec4(0.12f, 0.12f, 0.15f, 0.90f);

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
}


VOID c_overlay::update_style()
{
	auto& style = ImGui::GetStyle();
	auto& imgui_io = ImGui::GetIO();

	style = ImGuiStyle();

	set_style_colors();

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

	const auto menu_scale = persistentConfig.mainWindow.scale;
	const auto bomb_scale = persistentConfig.bombWindow.scale;

	imgui_io.Fonts->ClearFonts();

	// --- Font Laden (dein Code) ---
	font = imgui_io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 16.0f * menu_scale);
	titleFont = imgui_io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 20.0f * menu_scale);
	bombFont1 = imgui_io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 25.0f * bomb_scale);
	bombFont2 = imgui_io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 18.0f * bomb_scale);

	style.ScaleAllSizes(menu_scale);

	ImGui_ImplDX11_InvalidateDeviceObjects();
	ImGui_ImplDX11_CreateDeviceObjects();
}


VOID c_overlay::init_imgui()
{
	IMGUI_CHECKVERSION();

	imguiContext = ImGui::CreateContext();
	ImGui::SetCurrentContext(imguiContext);

	auto& imgui_io = ImGui::GetIO();

	imgui_io.IniFilename = nullptr;
	imgui_io.LogFilename = nullptr;

	ImGui_ImplWin32_Init(window_handle);
	ImGui_ImplDX11_Init(d3d_device, device_context);

	update_style();

	SetWindowLongPtr(window_handle, GWLP_WNDPROC, (LONG_PTR)WndProc_custom);
}

VOID c_overlay::dest_imgui()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext(imguiContext);

	imguiContext = nullptr;
	font = nullptr;
	titleFont = nullptr;
}
