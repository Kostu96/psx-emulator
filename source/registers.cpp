#include "registers.h"

void Registers::reset()
{
    for (unsigned int i = 0; i < SIZE; ++i)
        m_regs[i] = 0xDEADBEEF;
}
