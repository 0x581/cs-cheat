#include "../../overlay.hpp"

#include "CheatBase/colors.hpp"

#include "config.hpp"
#include "render_data.hpp"

void c_overlay::draw_cheats()
{
    const auto &config = cfg.config;

    if (config.activeCheats.esp)
    {
        float dynamicGlowSize = 0.0f;
        if (config.esp.boxGlow)
        {
            dynamicGlowSize = 5.0f + sin((float)ImGui::GetTime() * 2.0f) * 3.0f;
        }

        const ImU32 enemyColorU32 = ImGui::ColorConvertFloat4ToU32(config.esp.colorEnemyVisible);
        const ImU32 teamColorU32 = ImGui::ColorConvertFloat4ToU32(config.esp.colorTeamVisible);

        for (const auto &espRenderData : renderData.esp.data)
        {
            if (!espRenderData.isValid)
                continue;

            const auto friendly = espRenderData.isFriendly;

            if (!config.esp.renderFriendlies && friendly)
                continue;

            ImU32 currentColorU32 = friendly ? teamColorU32 : enemyColorU32;

            const auto min = espRenderData.min;
            const auto max = espRenderData.max;
            if (!(in_screen(min) || in_screen(max)))
                continue;

            const auto line_width = 1 / (espRenderData.distance) * 200 * config.esp.boneWidth;

            if (config.esp.bone)
            {
                const BoneData &bones = espRenderData.bones;

                line(bones.head, bones.neck, currentColorU32, line_width);
                line(bones.neck, bones.spine4, currentColorU32, line_width);
                line(bones.spine4, bones.spine3, currentColorU32, line_width);
                line(bones.spine3, bones.spine2, currentColorU32, line_width);
                line(bones.spine2, bones.spine1, currentColorU32, line_width);
                line(bones.spine1, bones.hip, currentColorU32, line_width);
                line(bones.spine4, bones.l_shoulder, currentColorU32, line_width);
                line(bones.l_shoulder, bones.l_arm, currentColorU32, line_width);
                line(bones.l_arm, bones.l_wrist, currentColorU32, line_width);
                line(bones.l_wrist, bones.l_hand, currentColorU32, line_width);
                line(bones.spine4, bones.r_shoulder, currentColorU32, line_width);
                line(bones.r_shoulder, bones.r_arm, currentColorU32, line_width);
                line(bones.r_arm, bones.r_wrist, currentColorU32, line_width);
                line(bones.r_wrist, bones.r_hand, currentColorU32, line_width);
                line(bones.hip, bones.l_hip, currentColorU32, line_width);
                line(bones.l_hip, bones.l_knee, currentColorU32, line_width);
                line(bones.l_knee, bones.l_foot, currentColorU32, line_width);
                line(bones.hip, bones.r_hip, currentColorU32, line_width);
                line(bones.r_hip, bones.r_knee, currentColorU32, line_width);
                line(bones.r_knee, bones.r_foot, currentColorU32, line_width);
            }

            if (config.esp.box)
            {
                const auto bounding_rect = ImRect(min, max);

                if (config.esp.boxGlow)
                    rect_with_glow(bounding_rect, currentColorU32, dynamicGlowSize * line_width, bounding_rect.GetHeight() / 8.f);
                else
                    rect(bounding_rect, currentColorU32, 2.0f);
            }

            if (config.esp.name)
            {
                const auto name_text = espRenderData.playerName;
                const auto text_size = esp_font->CalcTextSizeA(esp_font->FontSize, FLT_MAX, -1.0f, name_text, NULL, NULL);
                const auto text_pos = ImVec2{(max.x + min.x) / 2.f - (text_size.x / 2.f), max.y};
                text(text_pos, name_text, currentColorU32, esp_font, true);
            }

            if (config.esp.health)
            {
                const auto health_max = espRenderData.maxHealth;
                const auto health = espRenderData.health;
                const auto health_ratio = static_cast<float>(health) / static_cast<float>(health_max);

                const auto health_color = ImGui::ColorConvertFloat4ToU32(friendly ? config.esp.colorTeamVisible : config.esp.colorEnemyVisible);

                const float bar_width = 4.f;
                const float bar_spacing = 4.f;
                const auto height = max.y - min.y;
                const auto border_width = 1.f;

                const auto left = min.x - bar_width - bar_spacing;
                const auto top = min.y;
                const auto right = left + bar_width;
                const auto bottom = top + height;

                const auto health_height = height * health_ratio;
                const auto top_health = bottom - health_height;

                rect_filled(ImRect(left, top_health, right, bottom), health_color);
                rect(ImRect(left, top, right, bottom), rgbColors::black, border_width);
                char health_text[16];
                snprintf(health_text, sizeof(health_text), "%d", health);

                const auto health_text_size = esp_font->CalcTextSizeA(esp_font->FontSize, FLT_MAX, -1.0f, health_text, NULL, NULL);

                const auto health_text_pos = ImVec2{
                    left - health_text_size.x - 2.f,
                    top + (height / 15.f) - (health_text_size.y / 2.f)};
                text(health_text_pos, health_text, health_color, esp_font, true);
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
