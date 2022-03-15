#include "cpu.h"
#include "instruction_set.h"

#include <iostream>

void CPU::runNextInstruction()
{
    uint32_t inst = load32(PC);
    PC += 4;
    decodeAndExecute(inst);
}

void CPU::decodeAndExecute(Instruction instruction)
{
    uint32_t opcode = instruction.opcode();
    InstructionSet::Fns[opcode](*this, instruction);
}
