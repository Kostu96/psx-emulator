#include "memory_map.h"

#include <iostream>

uint8_t MemoryMap::load8(uint32_t address) const
{
    uint32_t absAddr = maskRegion(address);
    uint32_t offset;

    if (BIOS_RANGE.contains(absAddr, offset))
        return m_bios.load8(offset);

    if (RAM_RANGE.contains(absAddr, offset))
        return m_ram.load8(offset);

    if (EXPANSION1_RANGE.contains(absAddr, offset))
        return 0xFF; // To indicate that there is no EXPANSION1

    std::cerr << "Unhandled access when loading from: " << std::hex << address << std::dec << '\n';
    return 0xDE;
}

uint32_t MemoryMap::load32(uint32_t address) const
{
    if (address % 4 != 0)
        std::cerr << "Unaligned access when loading from: " << std::hex << address << std::dec << '\n';

    uint32_t absAddr = maskRegion(address);
    uint32_t offset;
    
    if (BIOS_RANGE.contains(absAddr, offset))
        return m_bios.load32(offset);

    if (RAM_RANGE.contains(absAddr, offset))
        return m_ram.load32(offset);

    std::cerr << "Unhandled access when loading from: " << std::hex << address << std::dec << '\n';
    return 0xDEADBEEF;
}

void MemoryMap::store8(uint32_t address, uint16_t value)
{
    if (address % 2 != 0)
        std::cerr << "Unaligned access when storing to: " << std::hex << address << std::dec << '\n';

    uint32_t absAddr = maskRegion(address);
    uint32_t offset;

    if (RAM_RANGE.contains(absAddr, offset)) {
        m_ram.store8(offset, value);
        return;
    }

    if (EXPANSION2_RANGE.contains(absAddr, offset)) {
        std::cerr << "Unhandled write to EXPANSION2: " << std::hex << address << std::dec << '\n';
        return; // Ignore EXPANSION2
    }

    std::cerr << "Unhandled access when storing to: " << std::hex << address << std::dec << '\n';
}

void MemoryMap::store16(uint32_t address, uint16_t value)
{
    if (address % 2 != 0)
        std::cerr << "Unaligned access when storing to: " << std::hex << address << std::dec << '\n';

    uint32_t absAddr = maskRegion(address);
    uint32_t offset;

    if (SPU_RANGE.contains(absAddr, offset)) {
        std::cerr << "Unhandled write to SPU: " << std::hex << address << std::dec << '\n';
        return; // Ignore SPU for now
    }

    std::cerr << "Unhandled access when storing to: " << std::hex << address << std::dec << '\n';
}

void MemoryMap::store32(uint32_t address, uint32_t value)
{
    if (address % 4 != 0)
        std::cerr << "Unaligned access when storing to: " << std::hex << address << std::dec << '\n';

    uint32_t absAddr = maskRegion(address);
    uint32_t offset;

    if (RAM_RANGE.contains(absAddr, offset)) {
        m_ram.store32(offset, value);
        return;
    }

    if (CACHE_CONTROL_RANGE.contains(absAddr, offset)) {
        std::cerr << "Unhandled write to CACHE_CONTROL: " << std::hex << address << std::dec << '\n';
        return; // Ignore stores to CACHE_CONTROL
    }

    if (SYS_CONTROL_RANGE.contains(absAddr, offset)) {
        switch (offset) {
        case 0:
            std::cerr << "Write to expansion 1 base address!\n";
            break;
        case 4:
            std::cerr << "Write to expansion 2 base address!\n";
            break;
        default:
            std::cerr << "Unhandled write to SYS_CONTROL: " << std::hex << address << std::dec << '\n';
        }
        return; // Ignore stores to MEMORY_CONTROL
    }

    if (RAM_SIZE_RANGE.contains(absAddr, offset)) {
        std::cerr << "Unhandled write to RAM_SIZE: " << std::hex << address << std::dec << '\n';
        return; // Ignore stores to RAM_SIZE
    }
    
    std::cerr << "Unhandled access when storing to: " << std::hex << address << std::dec << '\n';
}

uint32_t MemoryMap::maskRegion(uint32_t address)
{
    uint32_t index = address >> 29;
    return address & REGION_MASK[index];
}

const uint32_t MemoryMap::REGION_MASK[] = {
    // KUSEG 2048MB
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    // KSEG0 512MB
    0x7FFFFFFF,
    // KSEG1 512MB
    0x1FFFFFFF,
    // KSEG2 1024MB
    0xFFFFFFFF, 0xFFFFFFFF
};
const AddressRange MemoryMap::RAM_RANGE{ 0x00000000, 2 * 1024 * 1024 };
const AddressRange MemoryMap::BIOS_RANGE{ 0x1FC00000, 512 * 1024 };
const AddressRange MemoryMap::SYS_CONTROL_RANGE{ 0x1F801000, 36 };
const AddressRange MemoryMap::SPU_RANGE{ 0x1F801C00, 640 };
const AddressRange MemoryMap::RAM_SIZE_RANGE{ 0x1F801060, 4 };
const AddressRange MemoryMap::CACHE_CONTROL_RANGE{ 0xFFFE0130, 4 };
const AddressRange MemoryMap::EXPANSION1_RANGE{ 0x1F000000, 8 * 1024 * 1024 };
const AddressRange MemoryMap::EXPANSION2_RANGE{ 0x1F802000, 66 };
