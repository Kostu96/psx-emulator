#pragma once
#include "memory_map.h"
#include "registers.h"

class CPU
{
public:
	CPU() = default;
    void reset();
    void runNextInstruction();
    const Registers& getRegs() const { return m_regs; }
private:
    void decodeAndExecute(Instruction instruction);
    uint32_t load32(uint32_t address) { return m_memoryMap.load32(address); }
    void store32(uint32_t address, uint32_t value) { m_memoryMap.store32(address, value); }

    uint32_t m_PC = 0xBFC00000; // BIOS start
    uint32_t m_nextInstruction = 0x0; // NOP
    Registers m_regs;
    MemoryMap m_memoryMap;

    friend class InstructionSet;
};
