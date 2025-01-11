#pragma once

#include <thread>


#include "memory.hpp"
#include "offsets.hpp"

#ifdef ENABLE_ANTIFLASH

void static antiflash(const Memory* mem, uintptr_t client)
{
	
}

#else

static void antiflash(const Memory* mem, uintptr_t client) {}

#endif