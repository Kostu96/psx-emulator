#include "instruction_set.h"
#include "cpu.h"

#include <iostream>

void InstructionSet::invalid(CPU& cpu, Instruction instruction)
{
    std::cerr << "Unhandled instruction: " << std::hex << instruction.opcode() << std::dec << '\n';
}

void InstructionSet::SSL(CPU& cpu, Instruction instruction)
{
    uint32_t source = instruction.reg1();
    uint32_t target = instruction.reg3();
    uint32_t value = cpu.regs.get(source);
    value <<= instruction.shift();
    cpu.regs.set(target, value);
}

void InstructionSet::J(CPU& cpu, Instruction instruction)
{
    cpu.PC = (cpu.PC & 0xF0000000) | (instruction.imm_jump() << 2); // TODO: move this shift to imm_jump()?
}

void InstructionSet::ADDIU(CPU& cpu, Instruction instruction)
{
    uint32_t target = instruction.reg1();
    uint32_t source = instruction.reg2();
    uint32_t value = cpu.regs.get(source) + instruction.imm_se();
    cpu.regs.set(target, value);
}

void InstructionSet::ORI(CPU& cpu, Instruction instruction)
{
    uint32_t target = instruction.reg2();
    uint32_t value = cpu.regs.get(target);
    value |= instruction.imm();
    cpu.regs.set(target, value);
}

void InstructionSet::LUI(CPU& cpu, Instruction instruction)
{
	cpu.regs.set(instruction.reg2(), instruction.imm() << 16);
}

void InstructionSet::SW(CPU& cpu, Instruction instruction)
{
    cpu.store32(cpu.regs.get(instruction.reg1()) + instruction.imm_se(), instruction.reg2());
}

InstructionSet::InstFuncType InstructionSet::Fns[] = {
    SSL,     // 000000
    invalid, // 000001
    invalid, // 000010
    invalid, // 000011
    invalid, // 000100
    invalid, // 000101
    invalid, // 000110
    invalid, // 000111
    invalid, // 001000
    ADDIU,   // 001001
    invalid, // 001010
    invalid, // 001011
    invalid, // 001100
    ORI,     // 001101
    invalid, // 001110
    LUI,     // 001111
    invalid, // 010000
    invalid, // 010001
    invalid, // 010010
    invalid, // 010011
    invalid, // 010100
    invalid, // 010101
    invalid, // 010110
    invalid, // 010111
    invalid, // 011000
    invalid, // 011001
    invalid, // 011010
    invalid, // 011011
    invalid, // 011100
    invalid, // 011101
    invalid, // 011110
    invalid, // 011111
    invalid, // 100000
    invalid, // 100001
    invalid, // 100010
    invalid, // 100011
    invalid, // 100100
    invalid, // 100101
    invalid, // 100110
    invalid, // 100111
    invalid, // 101000
    invalid, // 101001
    invalid, // 101010
    SW,      // 101011
    invalid, // 101100
    invalid, // 101101
    invalid, // 101110
    invalid, // 101111
    invalid, // 110000
    invalid, // 110001
    invalid, // 110010
    invalid, // 110011
    invalid, // 110100
    invalid, // 110101
    invalid, // 110110
    invalid, // 110111
    invalid, // 111000
    invalid, // 111001
    invalid, // 111010
    invalid, // 111011
    invalid, // 111100
    invalid, // 111101
    invalid, // 111110
    invalid  // 111111
};
