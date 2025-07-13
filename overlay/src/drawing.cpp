#include "../overlay.h"

#include <string>

using std::string;

VOID c_overlay::rect(const ImVec2& min, const ImVec2& max, ImU32 color, const FLOAT line_width)
{
	draw_list->AddRect(min, max, color, 0.0f, 0, line_width);
}

VOID c_overlay::rect_filled(const ImVec2& min, const ImVec2& max, ImU32 color)
{
	draw_list->AddRectFilled(min, max, color);
}

void c_overlay::box_with_glow(const ImVec2& min, const ImVec2& max, ImU32 color, FLOAT line_width, FLOAT glow_size)
{
	ImDrawList* draw_list = ImGui::GetBackgroundDrawList();


	float alpha = (color >> 24 & 0xFF) / 255.0f;
	ImVec4 color_vec = ImGui::ColorConvertU32ToFloat4(color);


	float glow_intensity = 0.3f;


	for (int i = 1; i <= glow_size; i++)
	{

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

VOID c_overlay::line(const ImVec2& point1, const ImVec2 point2, ImU32 color, const FLOAT line_width)
{
	if ((point1.x >= 0.f && point1.y >= 0.f && point1.x < window_width && point1.y < window_height)
		|| (point2.x >= 0.f && point2.y >= 0.f && point2.x < window_width && point2.y < window_height))
		draw_list->AddLine(point1, point2, color, line_width);
}

//aimbot fov circle
VOID c_overlay::circle(const ImVec2& point, const FLOAT radius, ImU32 color)
{
	draw_list->AddCircle(point, radius, color, 200, 3.f); // num segments is how circular the circle actually is
}

//esp names
VOID c_overlay::text(const ImVec2& pos, const string& text, ImU32 color, bool outline)
{
	if (outline)
	{
		draw_list->AddText(font, font->FontSize, { pos.x + 1, pos.y }, IM_COL32_BLACK, text.c_str());
		draw_list->AddText(font, font->FontSize, { pos.x - 1, pos.y }, IM_COL32_BLACK, text.c_str());
		draw_list->AddText(font, font->FontSize, { pos.x, pos.y + 1 }, IM_COL32_BLACK, text.c_str());
		draw_list->AddText(font, font->FontSize, { pos.x, pos.y - 1 }, IM_COL32_BLACK, text.c_str());
	}
	draw_list->AddText(font, font->FontSize, pos, color, text.c_str());
}

VOID c_overlay::esp_text(const string& text, const ImVec2& pos, ImU32 color, bool outline)
{
	if (outline)
	{
		draw_list->AddText(esp_font, esp_font->FontSize, { pos.x + 1, pos.y }, IM_COL32_BLACK, text.c_str());
		draw_list->AddText(esp_font, esp_font->FontSize, { pos.x - 1, pos.y }, IM_COL32_BLACK, text.c_str());
		draw_list->AddText(esp_font, esp_font->FontSize, { pos.x, pos.y + 1 }, IM_COL32_BLACK, text.c_str());
		draw_list->AddText(esp_font, esp_font->FontSize, { pos.x, pos.y - 1 }, IM_COL32_BLACK, text.c_str());
	}
	draw_list->AddText(esp_font, esp_font->FontSize, pos, color, text.c_str());
}

// Filled circle. creates a gradient from middle color to outer color
VOID c_overlay::radial_gradient(const ImVec2& center, float radius, ImU32 col_in, ImU32 col_out)
{
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

	for (int n = 0; n < count; n++)
	{
		draw_list->PrimWriteIdx((ImDrawIdx)(vtx_base));
		draw_list->PrimWriteIdx((ImDrawIdx)(vtx_base + 1 + n));
		draw_list->PrimWriteIdx((ImDrawIdx)(vtx_base + 1 + ((n + 1) % count)));
	}
	draw_list->_Path.Size = 0;
}

BOOL c_overlay::in_screen(const ImVec2& pos)
{
	return !(pos.x > window_width || pos.x<0 || pos.y>window_height || pos.y < 0);
}
