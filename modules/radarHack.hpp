#pragma once

#include "activeCheats.hpp"

#include "../memory.hpp"
#include "../offsets.hpp"

#ifdef ENABLE_RADAR

void radarHack(const Memory &mem)
{
}

#else
void radarHack(const Memory &mem) {}
#endif
