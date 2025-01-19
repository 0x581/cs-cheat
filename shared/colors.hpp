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
    constexpr RGB black { 0, 0, 0 };
    constexpr RGB white{ 255, 255, 255 };
    constexpr RGB red{ 255, 0, 0 };
    constexpr RGB green{ 0, 255, 0 };
    constexpr RGB blue{ 0, 0, 255 };
    constexpr RGB yellow{ 255, 255, 0 };
}

namespace vec4Colors {
    constexpr ImVec4 red{ 1.00f, 0.04f, 0.00f, 0.71f };
}
