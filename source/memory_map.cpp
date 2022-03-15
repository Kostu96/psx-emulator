#include "memory_map.h"

#include <iostream>

uint32_t MemoryMap::load32(uint32_t address) const
{
    if (address % 4 != 0)
        std::cerr << "Unaligned access when loading from: " << std::hex << address << std::dec << '\n';

    uint32_t offset;
    if (BIOS::RANGE.contains(address, offset))
        return m_bios.load32(offset);

    std::cerr << "Unhandled access when loading from: " << std::hex << address << std::dec << '\n';
    return -1;
}

void MemoryMap::store32(uint32_t address, uint32_t value)
{
    if (address % 4 != 0)
        std::cerr << "Unaligned access when storing to: " << std::hex << address << std::dec << '\n';

    std::cerr << "Unhandled access when storing to: " << std::hex << address << std::dec << '\n';
}
