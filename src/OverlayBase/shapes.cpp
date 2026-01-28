#include "overlay_base.hpp"

#include <string>

using std::string;

void OverlayBase::rect(const ImRect &rect, ImU32 color, float line_width, float rounding)
{
    draw_list->AddRect(rect.Min, rect.Max, color, rounding, 0, line_width);
}

void OverlayBase::rect_filled(const ImRect &rect, ImU32 color, float rounding)
{
    draw_list->AddRectFilled(rect.Min, rect.Max, color, rounding);
}

void OverlayBase::rect_with_glow(const ImRect &rect, ImU32 color, float glow_size, float rounding)
{
    constexpr float step_size = 1.5f;

    const auto &min = rect.Min;
    const auto &max = rect.Max;

    const ImVec4 color_vec = ImGui::ColorConvertU32ToFloat4(color);

    for (float i = 0.f; i <= glow_size; i += step_size)
    {
        float t = i / glow_size;
        float glow_alpha = color_vec.w * (1.0f - t) * (1.0f - t);
        ImU32 glow_color = ImGui::ColorConvertFloat4ToU32(ImVec4(color_vec.x, color_vec.y, color_vec.z, glow_alpha));

        const auto curr_rounding = rounding + i;
        // const auto curr_rounding = rounding + i / 2.f;
        // const auto curr_rounding = rounding * (1 + i / glow_size);

        draw_list->AddRect(min - ImVec2(i, i), max + ImVec2(i, i), glow_color, curr_rounding, 0, step_size);
    }
}

void OverlayBase::line(const ImVec2 &point1, const ImVec2 &point2, ImU32 color, const float line_width)
{
    if (in_screen(point1) || in_screen(point2))
        draw_list->AddLine(point1, point2, color, line_width);
}

void OverlayBase::circle(const ImVec2 &point, const float radius, ImU32 color)
{
    draw_list->AddCircle(point, radius, color, 200, 3.f); // num segments is how circular the circle actually is
}

void OverlayBase::text(const ImVec2 &pos, const string &text, ImU32 color, ImFont *font, bool outline)
{
    if (outline)
    {
        draw_list->AddText(font, font->FontSize, {pos.x + 1, pos.y}, IM_COL32_BLACK, text.c_str());
        draw_list->AddText(font, font->FontSize, {pos.x - 1, pos.y}, IM_COL32_BLACK, text.c_str());
        draw_list->AddText(font, font->FontSize, {pos.x, pos.y + 1}, IM_COL32_BLACK, text.c_str());
        draw_list->AddText(font, font->FontSize, {pos.x, pos.y - 1}, IM_COL32_BLACK, text.c_str());
    }

    draw_list->AddText(font, font->FontSize, pos, color, text.c_str());
}
