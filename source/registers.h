#pragma once
#include <cstdint>

class Registers
{
public:
    void set(uint32_t reg, uint32_t value)
    {
        regs[reg] = value;
        regs[0] = 0;
    }

    uint32_t get(uint32_t reg) const { return regs[reg]; }
private:
	static const size_t SIZE = 32;
	uint32_t regs[SIZE]{};
};
