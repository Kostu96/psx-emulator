#include "cpu.h"
#include "instruction_set.h"

#include <iostream>

void CPU::reset()
{
    m_PC = 0xBFC00000; // BIOS start
    m_nextInstruction = 0x0; // NOP
    m_regs.reset();
}

void CPU::runNextInstruction()
{
    uint32_t inst = m_nextInstruction;
    m_nextInstruction = load32(m_PC);
    m_PC += 4;
    decodeAndExecute(inst);
}

void CPU::decodeAndExecute(Instruction instruction)
{
    uint32_t opcode = instruction.opcode();
    InstructionSet::Fns[opcode](*this, instruction);
}
