#include "cpu.h"
#include "instruction_set.h"

#include <iostream>

void CPU::reset()
{
    m_currentPC = 0xDEADBEEF;
    m_PC = 0xBFC00000; // BIOS start
    m_nextPC = 0xBFC00004;
    m_SR = 0;
    m_CAUSE = 0xDEADBEEF;
    m_EPC = 0xDEADBEEF;
    m_HI = 0xDEADBEEF;
    m_LO = 0xDEADBEEF;
    m_inputRegs.reset();
    m_outputRegs.reset();
    m_pendingLoad = { 0, 0 };
}

void CPU::runNextInstruction()
{
    uint32_t inst = load32(m_PC);
    m_PC = m_nextPC;
    m_nextPC += 4;

    setReg(m_pendingLoad.reg, m_pendingLoad.value);
    m_pendingLoad = { 0, 0 };

    decodeAndExecute(inst);

    m_inputRegs = m_outputRegs;
}

void CPU::decodeAndExecute(Instruction instruction)
{
    uint32_t opcode = instruction.opcode();
    InstructionSet::Fns[opcode](*this, instruction);
}
