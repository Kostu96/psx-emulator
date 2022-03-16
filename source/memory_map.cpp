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

    uint32_t offset;
    if (MEMORY_CONTROL_RANGE.contains(address, offset)) {
        switch (offset) {
        case 0:
            std::cerr << "Write to expansion 1 base address!\n";
            break;
        case 4:
            std::cerr << "Write to expansion 2 base address!\n";
            break;
        default:
            std::cerr << "Unhandled write to MEMORY CONTROL: " << std::hex << address << std::dec << '\n';
        }
        return; // Ignore stores to MEMORY_CONTROL
    }
    else if (RAM_SIZE_RANGE.contains(address, offset)) {
        return; // Ignore stores to RAM_SIZE
    }
    
    std::cerr << "Unhandled access when storing to: " << std::hex << address << std::dec << '\n';
}

const AddressRange MemoryMap::MEMORY_CONTROL_RANGE{ 0x1F801000, 36 };
const AddressRange MemoryMap::RAM_SIZE_RANGE{ 0x1F801060, 4 };
