#pragma once

#include "../lib/pch.h"

typedef struct
{
    unsigned short R;
    unsigned short G;
    unsigned short B;

    constexpr ImU32 toImU32() const {
        return IM_COL32(this->R, this->G, this->B, 255);
    }
} RGB;

namespace colors {
    constexpr RGB black { 0, 0, 0 };
    constexpr RGB white{ 255, 255, 255 };
    constexpr RGB red{ 255, 0, 0 };
    constexpr RGB green{ 0, 255, 0 };
    constexpr RGB blue{ 0, 0, 255 };
    constexpr RGB yellow{ 255, 255, 0 };
}