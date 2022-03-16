#include "instruction_set.h"
#include "cpu.h"

#include <iostream>

void InstructionSet::invalid(CPU& cpu, Instruction instruction)
{
    std::cerr << "Unhandled instruction: " << std::hex << instruction.opcode() << std::dec << '\n';
}

void InstructionSet::zero(CPU& cpu, Instruction instruction)
{
    uint32_t subfunction = instruction.subfn();
    switch (subfunction) {
    case 0b000000:
        SSL(cpu, instruction);
        break;
    case 0b100101:
        OR(cpu, instruction);
        break;
    default:
        std::cerr << "Unhandled zero instruction: " << std::hex << instruction.subfn() << std::dec << '\n';
    }
}

void InstructionSet::SSL(CPU& cpu, Instruction instruction)
{
    RegisterIndex source = instruction.regT();
    RegisterIndex target = instruction.regD();
    uint32_t value = cpu.getReg(source);
    value <<= instruction.shift();
    cpu.setReg(target, value);
}

void InstructionSet::OR(CPU& cpu, Instruction instruction)
{
    RegisterIndex target = instruction.regS();
    uint32_t value = cpu.getReg(target);
    value |= instruction.imm();
    cpu.setReg(target, value);
}

void InstructionSet::J(CPU& cpu, Instruction instruction)
{
    cpu.m_PC = (cpu.m_PC & 0xF0000000) | (instruction.imm_jump() << 2); // TODO: move this shift to imm_jump()?
}

void InstructionSet::BNE(CPU& cpu, Instruction instruction)
{
    uint32_t imm = instruction.imm_se();
    RegisterIndex s = instruction.regS();
    RegisterIndex t = instruction.regT();

    if (cpu.getReg(s) != cpu.getReg(t)) {
        cpu.m_PC += (imm << 2);
        cpu.m_PC -= 4;
    }
}

void InstructionSet::ADDI(CPU& cpu, Instruction instruction)
{
    int32_t imm = instruction.imm_se();
    RegisterIndex t = instruction.regT();
    RegisterIndex s = instruction.regS();
    int32_t value = cpu.getReg(s);
    value += imm; // TODO: check for overflow
    cpu.setReg(t, value);
}

void InstructionSet::ADDIU(CPU& cpu, Instruction instruction)
{
    RegisterIndex target = instruction.regT();
    RegisterIndex source = instruction.regS();
    uint32_t value = cpu.getReg(source) + instruction.imm_se();
    cpu.setReg(target, value);
}

void InstructionSet::ORI(CPU& cpu, Instruction instruction)
{
    RegisterIndex target = instruction.regS();
    uint32_t value = cpu.getReg(target);
    value |= instruction.imm();
    cpu.setReg(target, value);
}

void InstructionSet::LUI(CPU& cpu, Instruction instruction)
{
    uint32_t imm = instruction.imm();
    RegisterIndex target = instruction.regT();
    uint32_t value = imm << 16;
	cpu.setReg(target, value);
}

void InstructionSet::cop0(CPU& cpu, Instruction instruction)
{
    uint32_t copfn = instruction.copfn();
    switch (copfn) {
    case 0b00100:
        MTC0(cpu, instruction);
        break;
    default:
        std::cerr << "Unhandled cop0 instruction: " << std::hex << instruction.copfn() << std::dec << '\n';
    }
}

void InstructionSet::MTC0(CPU& cpu, Instruction instruction)
{
    // TODO: temp
    RegisterIndex cpuReg = instruction.regT();
    RegisterIndex copReg = instruction.regD();
    uint32_t value = cpu.getReg(cpuReg);

    if (copReg.index == 12)
        cpu.m_SR = value;
    else {
        std::cerr << "Unhandled MTC0 instruction\n";
    }
}

void InstructionSet::LW(CPU& cpu, Instruction instruction)
{
    if (cpu.m_SR & 0x10000)
        return; // isolated cache bit is set

    uint32_t imm = instruction.imm_se();
    RegisterIndex t = instruction.regT();
    RegisterIndex s = instruction.regS();
    uint32_t address = cpu.getReg(s) + imm;
    uint32_t value = cpu.load32(address);
    cpu.m_pendingLoad = { t, address };
}

void InstructionSet::SW(CPU& cpu, Instruction instruction)
{
    if (cpu.m_SR & 0x10000)
        return; // isolated cache bit is set

    uint32_t imm = instruction.imm_se();
    RegisterIndex t = instruction.regT();
    RegisterIndex s = instruction.regS();
    uint32_t address = cpu.getReg(s) + imm;
    uint32_t value = cpu.getReg(t);
    cpu.store32(address, value);
}

InstructionSet::InstFuncType InstructionSet::Fns[] = {
    zero,    // 000000
    invalid, // 000001
    J,       // 000010
    invalid, // 000011
    invalid, // 000100
    BNE,     // 000101
    invalid, // 000110
    invalid, // 000111
    ADDI,    // 001000
    ADDIU,   // 001001
    invalid, // 001010
    invalid, // 001011
    invalid, // 001100
    ORI,     // 001101
    invalid, // 001110
    LUI,     // 001111
    cop0,    // 010000
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
    LW,      // 100011
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
