#include "timers.h"

#include <iostream>

uint32_t Timers::load32(uint32_t offset) const
{
	std::cerr << "Unhandled load32 from TIMERS: " << std::hex << offset << std::dec << '\n';
	__debugbreak();
	return 0;
}

void Timers::store16(uint32_t offset, uint16_t value)
{
	std::cerr << "Unhandled store16 to TIMERS: " << std::hex << offset << ':' << value << std::dec << '\n';
	__debugbreak();
}

void Timers::store32(uint32_t offset, uint32_t value)
{
	std::cerr << "Unhandled store32 to TIMERS: " << std::hex << offset << ':' << value << std::dec << '\n';
	__debugbreak();
}
