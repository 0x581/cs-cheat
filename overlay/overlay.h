#pragma once

#include "imgui.h"

constexpr float menu_width = 650.f;
constexpr float menu_height = 500.f;

constexpr float bomb_width = 300;
constexpr float bomb_height = 100;

class c_overlay
{
private:
	HWND window_handle;
	FLOAT window_width, window_height;
	bool window_scale_changed;
	ImVec2 menuSize;
	ImVec2 bombSize;

	ID3D11Device* d3d_device;
	ID3D11DeviceContext* device_context;
	IDXGISwapChain* swap_chain;
	ID3D11RenderTargetView* render_target_view;

	ImDrawList* draw_list;
	ImGuiContext* imguiContext;

	ImFont* titleFont;
	ImFont* font;
	ImFont* bombFont1;
	ImFont* bombFont2;

	bool show_menu;

public:
	bool exit;
	bool ready_to_close;

private:
	template <typename T>
	inline VOID safe_release(T*& p);

	BOOL init_device();
	VOID dest_device();


	VOID update_style();

	VOID init_imgui();
	VOID dest_imgui();

	VOID init_render_target();
	VOID dest_render_target();
	const VOID draw_cheats();
	const VOID render();

	VOID update_on_resolution_change();

	VOID state_handler();

	VOID draw_menu();
	VOID draw_bomb();

	VOID HandleInput();

	VOID init_draw_list();
public:
	c_overlay();
	~c_overlay();

	BOOL msg_loop();

	VOID clear_screen();
	//					 //
	// Drawing functions //
	//					 //
	VOID rect(const ImVec2& min, const ImVec2& max, ImU32 color, const FLOAT line_width = 2.f);
	VOID rect_filled(const ImVec2& min, const ImVec2& max, ImU32 color);
	VOID line(const ImVec2& point1, const ImVec2 point2, ImU32 color, const FLOAT line_width = 2.f);
	VOID circle(const ImVec2& point, const FLOAT radius, ImU32 color);
	VOID text(const ImVec2& pos, const std::string& text, ImU32 color, bool outline = true);
	VOID radial_gradient(const ImVec2& center, float radius, ImU32 col_in, ImU32 col_out);
	VOID box_with_glow(const ImVec2& min, const ImVec2& max, ImU32 color, FLOAT line_width, FLOAT glow_size);

	BOOL in_screen(const ImVec2& pos);
};
