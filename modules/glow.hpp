#pragma once

#include "activeCheats.hpp"

#include "../memory.hpp"
#include "../offsets.hpp"

#ifdef ENABLE_GLOW

void glowHack(const Memory* mem, uintptr_t client)
{

}

#else
void glowHack(const Memory* mem, uintptr_t client) {}
#endif
