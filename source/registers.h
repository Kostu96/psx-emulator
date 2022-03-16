#pragma once
#include <cstdint>

class Registers
{
public:
    void reset();

    void set(uint32_t reg, uint32_t value)
    {
        m_regs[reg] = value;
        m_regs[0] = 0;
    }

    uint32_t get(uint32_t reg) const { return m_regs[reg]; }
private:
	static const size_t SIZE = 32;
	uint32_t m_regs[SIZE]{};
};
