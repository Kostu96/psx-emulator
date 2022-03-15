#pragma once
#include "memory_map.h"
#include "registers.h"

class CPU
{
public:
	CPU() = default;

    void runNextInstruction();
private:
    void decodeAndExecute(Instruction instruction);
    uint32_t load32(uint32_t address) { return memoryMap.load32(address); }
    void store32(uint32_t address, uint32_t value) { memoryMap.store32(address, value); }

    uint32_t PC = 0xBFC00000;
    Registers regs;
    MemoryMap memoryMap;

    friend class InstructionSet;
};
