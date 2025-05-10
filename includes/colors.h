#pragma once

#include "../lib/pch.h"

struct RGB {
	unsigned short R;
	unsigned short G;
	unsigned short B;

	constexpr ImU32 toImU32() const {
		return IM_COL32(this->R, this->G, this->B, 255);
	}
};

namespace rgbColors {
	constexpr ImU32 black = IM_COL32(0, 0, 0, 255);    // Black
	constexpr ImU32 white = IM_COL32(255, 255, 255, 255);  // White
	constexpr ImU32 off_white = IM_COL32(240, 248, 255, 255);
	constexpr ImU32 red = IM_COL32(255, 0, 0, 255);    // Red
	constexpr ImU32 green = IM_COL32(0, 255, 0, 255);  // Green
	constexpr ImU32 blue = IM_COL32(0, 0, 255, 255);   // Blue
	constexpr ImU32 yellow = IM_COL32(255, 255, 0, 255);  // Yellow
	// Purple shades
	constexpr ImU32 purple1 = IM_COL32(153, 0, 204, 255); // Vivid purple
	constexpr ImU32 purple2 = IM_COL32(102, 0, 204, 255); // Darker purple

	constexpr ImU32 transparent_white = IM_COL32(255, 255, 255, 64);
}


namespace vec4Colors {
	constexpr ImVec4 red{ 1.00f, 0.04f, 0.00f, 0.71f };
	constexpr ImVec4 purple{ 0.50f, 0.00f, 0.50f, 0.71f };
}
