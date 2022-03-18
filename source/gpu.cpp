#include "gpu.h"

#include <iostream>

uint32_t GPU::load32(uint32_t offset) const
{
    return 0;
}

void GPU::store32(uint32_t offset, uint32_t value)
{
    switch (offset) {
    case 0:
        gp0write(value);
        return;
    case 4:
        gp1write(value);
        return;
    }
    
    std::cerr << "Store to ivalid GPU location: " << std::hex << offset << std::dec << '\n';
    abort();
}

void GPU::gp0write(uint32_t value)
{

}

void GPU::gp1write(uint32_t value)
{

}
