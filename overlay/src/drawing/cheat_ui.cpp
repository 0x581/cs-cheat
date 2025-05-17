#include "../../overlay.h"

#include <config.h>
#include <shared_variables.h>
#include <colors.h>

const void c_overlay::renderCheats() {
	if (config.activeCheats.esp)
	{
		float dynamicGlowSize = 0.0f;
		if (config.esp.boxGlow)
		{
			dynamicGlowSize = 5.0f + sin((float)ImGui::GetTime() * 2.0f) * 3.0f;
		}

		const ImU32 enemyColorU32 = ImGui::ColorConvertFloat4ToU32(config.esp.colorEnemyVisible);
		const ImU32 teamColorU32 = ImGui::ColorConvertFloat4ToU32(config.esp.colorTeamVisible);


		for (const auto& espRenderData : espData.data)
		{
			if (!espRenderData.isValid)
				continue;

			const auto friendly = espRenderData.isFriendly;

			if (!config.esp.renderFriendlies && friendly)
				continue;

			ImU32 currentColorU32 = friendly ? teamColorU32 : enemyColorU32;

			const auto min = espRenderData.min;
			const auto max = espRenderData.max;
			if (!((min.x >= 0.f && min.y >= 0.f && min.x < window_width && min.y < window_height) ||
				(max.x >= 0.f && max.y >= 0.f && max.x < window_width && max.y < window_height)))
				continue;

			if (config.esp.bone)
			{
				const BoneData& bones = espRenderData.bones;


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

			if (config.esp.box)
			{
				if (config.esp.boxGlow)
					box_with_glow(min, max, currentColorU32, 2.0f, dynamicGlowSize);
				else
					rect(min, max, currentColorU32);
			}

			if (config.esp.name)
			{
				const auto name_text = espRenderData.playerName;
				// const auto text_size = ImGui::CalcTextSize(name_text); 
				const auto text_pos = ImVec2{ (max.x + min.x) / 2.f - (ImGui::CalcTextSize(name_text).x / 2.f), max.y };
				text(text_pos, name_text, currentColorU32);
			}

			if (config.esp.health)
			{
				const auto health_max = espRenderData.maxHealth;
				const auto health = espRenderData.health;
				const auto health_ratio = static_cast<float>(health) / static_cast<float>(health_max);

				const auto health_color = ImGui::ColorConvertFloat4ToU32(friendly ? config.esp.colorTeamVisible : config.esp.colorEnemyVisible);

				const auto height = max.y - min.y;
				const auto width = (max.x - min.x) * 0.2f;
				const auto border_width = width * 0.1f;

				const auto left = min.x - width - 5.f;
				const auto top = min.y;

				const auto right = left + width;
				const auto bottom = top + height;

				const auto health_height = height * health_ratio;
				const auto top_health = bottom - health_height;

				rect_filled(ImVec2(left, top_health), ImVec2(right, bottom), health_color);
				rect(ImVec2(left, top), ImVec2(right, bottom), rgbColors::black, border_width);
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
