#pragma once
#include "memory_map.h"
#include "registers.h"

class CPU
{
public:
	CPU() = default;
    void reset();
    void runNextInstruction();
    uint32_t getReg(RegisterIndex index) const { return m_inputRegs.get(index); }
    void setReg(RegisterIndex index, uint32_t value) { m_outputRegs.set(index, value); }
private:
    struct Load {
        RegisterIndex reg;
        uint32_t value;
    };

    void decodeAndExecute(Instruction instruction);
    uint8_t load8(uint32_t address) { return m_memoryMap.load8(address); }
    uint32_t load32(uint32_t address) { return m_memoryMap.load32(address); }
    void store8(uint32_t address, uint8_t value) { m_memoryMap.store8(address, value); }
    void store16(uint32_t address, uint16_t value) { m_memoryMap.store16(address, value); }
    void store32(uint32_t address, uint32_t value) { m_memoryMap.store32(address, value); }

    uint32_t m_currentPC = 0xDEADBEEF;
    uint32_t m_PC = 0xBFC00000; // BIOS start
    uint32_t m_nextPC = 0xBFC00004;
    uint32_t m_SR = 0;
    uint32_t m_CAUSE = 0xDEADBEEF;
    uint32_t m_EPC = 0xDEADBEEF;
    uint32_t m_HI = 0xDEADBEEF;
    uint32_t m_LO = 0xDEADBEEF;
    Registers m_inputRegs;
    Registers m_outputRegs;
    Load m_pendingLoad{0, 0};
    MemoryMap m_memoryMap;

    friend class InstructionSet;
};
