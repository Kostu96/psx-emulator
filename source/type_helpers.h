#pragma once
#include <cstdint>

struct AddressRange
{
    uint32_t start;
    uint32_t size;

    bool contains(uint32_t address, uint32_t& offset) const
    {
        offset = address - start;
        return (address >= start) && (address < start + size);
    }
};

struct Instruction
{
    uint32_t dword;

    Instruction(uint32_t pDword) : dword(pDword) {}

    uint32_t opcode() const { return dword >> 26; }
    uint32_t reg1() const { return (dword >> 21) & 0x1F; }
    uint32_t reg2() const { return (dword >> 16) & 0x1F; }
    uint32_t imm() const { return dword & 0xFFFF; }
};
