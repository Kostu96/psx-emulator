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
    case 0x00:
        SLL(cpu, instruction);
        break;
    case 0x02:
        SRL(cpu, instruction);
        break;
    case 0x03:
        SRA(cpu, instruction);
        break;
    case 0x08:
        JR(cpu, instruction);
        break;
    case 0x09:
        JALR(cpu, instruction);
        break;
    case 0x0C:
        SYSCALL(cpu, instruction);
        break;
    case 0x10:
        MFHI(cpu, instruction);
        break;
    case 0x12:
        MFLO(cpu, instruction);
        break;
    case 0x1A:
        DIV(cpu, instruction);
        break;
    case 0x1B:
        DIVU(cpu, instruction);
        break;
    case 0x20:
        ADD(cpu, instruction);
        break;
    case 0x21:
        ADDU(cpu, instruction);
        break;
    case 0x23:
        SUBU(cpu, instruction);
        break;
    case 0x24:
        AND(cpu, instruction);
        break;
    case 0x25:
        OR(cpu, instruction);
        break;
    case 0x2A:
        SLT(cpu, instruction);
        break;
    case 0x2B:
        SLTU(cpu, instruction);
        break;
    default:
        std::cerr << "Unhandled zero instruction: " << std::hex << instruction.subfn() << std::dec << '\n';
    }
}

void InstructionSet::bxxxxx(CPU& cpu, Instruction instruction)
{
    uint32_t imm = instruction.imm_se();
    RegisterIndex s = instruction.regS();

    uint32_t isBGEZ = (instruction.dword >> 16) & 1;
    bool isLink = (instruction.dword >> 20) & 1;

    int32_t value = cpu.getReg(s);
    uint32_t test = value < 0;
    test = test ^ isBGEZ;

    if (test != 0) {
        if (isLink)
            cpu.setReg(31, cpu.m_PC);

        cpu.m_nextPC += (imm << 2);
        cpu.m_nextPC -= 4;
    }
}

void InstructionSet::SLL(CPU& cpu, Instruction instruction)
{
    RegisterIndex source = instruction.regT();
    RegisterIndex target = instruction.regD();
    uint32_t value = cpu.getReg(source);
    value <<= instruction.shift();
    cpu.setReg(target, value);
}

void InstructionSet::SRL(CPU& cpu, Instruction instruction)
{
    RegisterIndex source = instruction.regT();
    RegisterIndex target = instruction.regD();
    uint32_t value = cpu.getReg(source);
    value >>= instruction.shift();
    cpu.setReg(target, value);
}

void InstructionSet::SRA(CPU& cpu, Instruction instruction)
{
    RegisterIndex t = instruction.regT();
    RegisterIndex d = instruction.regD();
    int32_t value = cpu.getReg(t);
    value >>= instruction.shift();
    cpu.setReg(d, value);
}

void InstructionSet::JR(CPU& cpu, Instruction instruction)
{
    RegisterIndex s = instruction.regS();
    cpu.m_nextPC = cpu.getReg(s);
}

void InstructionSet::JALR(CPU& cpu, Instruction instruction)
{
    RegisterIndex d = instruction.regD();
    RegisterIndex s = instruction.regS();
    cpu.setReg(d, cpu.m_nextPC);
    cpu.m_nextPC = cpu.getReg(s);
}

void InstructionSet::SYSCALL(CPU& cpu, Instruction instruction)
{
    ;
}

void InstructionSet::MFHI(CPU& cpu, Instruction instruction)
{
    RegisterIndex d = instruction.regD();
    cpu.setReg(d, cpu.m_HI);
}

void InstructionSet::MFLO(CPU& cpu, Instruction instruction)
{
    RegisterIndex d = instruction.regD();
    cpu.setReg(d, cpu.m_LO);
}

void InstructionSet::DIV(CPU& cpu, Instruction instruction)
{
    RegisterIndex s = instruction.regS();
    RegisterIndex t = instruction.regT();
    int32_t n = cpu.getReg(s);
    int32_t d = cpu.getReg(t);
    if (d == 0) {
        // division by 0
        cpu.m_HI = n;
        cpu.m_LO = (n >= 0) ? 0xFFFFFFFF : 1;
    }
    else if (static_cast<uint32_t>(n) == 0x80000000 && d == -1) {
        cpu.m_HI = 0;
        cpu.m_LO = 0x80000000;
    }
    else {
        cpu.m_HI = n % d;
        cpu.m_LO = n / d;
    }
}

void InstructionSet::DIVU(CPU& cpu, Instruction instruction)
{
    RegisterIndex s = instruction.regS();
    RegisterIndex t = instruction.regT();
    uint32_t n = cpu.getReg(s);
    uint32_t d = cpu.getReg(t);
    if (d == 0) {
        // division by 0
        cpu.m_HI = n;
        cpu.m_LO = 0xFFFFFFFF;
    }
    else {
        cpu.m_HI = n % d;
        cpu.m_LO = n / d;
    }
}

void InstructionSet::ADD(CPU& cpu, Instruction instruction)
{
    RegisterIndex s = instruction.regS();
    RegisterIndex t = instruction.regT();
    RegisterIndex d = instruction.regD();
    uint32_t value = cpu.getReg(s) + cpu.getReg(t); // TODO: check for overflow
    cpu.setReg(d, value);
}

void InstructionSet::ADDU(CPU& cpu, Instruction instruction)
{
    RegisterIndex s = instruction.regS();
    RegisterIndex t = instruction.regT();
    RegisterIndex d = instruction.regD();
    uint32_t value = cpu.getReg(s) + cpu.getReg(t);
    cpu.setReg(d, value);
}

void InstructionSet::SUBU(CPU& cpu, Instruction instruction)
{
    RegisterIndex s = instruction.regS();
    RegisterIndex t = instruction.regT();
    RegisterIndex d = instruction.regD();

    uint32_t value = cpu.getReg(s) - cpu.getReg(t);
    cpu.setReg(d, value);
}

void InstructionSet::AND(CPU& cpu, Instruction instruction)
{
    RegisterIndex d = instruction.regD();
    RegisterIndex s = instruction.regS();
    RegisterIndex t = instruction.regT();
    uint32_t value = cpu.getReg(s) & cpu.getReg(t);
    cpu.setReg(d, value);
}

void InstructionSet::OR(CPU& cpu, Instruction instruction)
{
    RegisterIndex d = instruction.regD();
    RegisterIndex s = instruction.regS();
    RegisterIndex t = instruction.regT();
    uint32_t value = cpu.getReg(s) | cpu.getReg(t);
    cpu.setReg(d, value);
}

void InstructionSet::SLT(CPU& cpu, Instruction instruction)
{
    RegisterIndex d = instruction.regD();
    RegisterIndex s = instruction.regS();
    RegisterIndex t = instruction.regT();
    uint32_t value = static_cast<int32_t>(cpu.getReg(s)) < static_cast<int32_t>(cpu.getReg(t));
    cpu.setReg(d, value);
}

void InstructionSet::SLTU(CPU& cpu, Instruction instruction)
{
    RegisterIndex d = instruction.regD();
    RegisterIndex s = instruction.regS();
    RegisterIndex t = instruction.regT();
    uint32_t value = cpu.getReg(s) < cpu.getReg(t);
    cpu.setReg(d, value);
}

void InstructionSet::J(CPU& cpu, Instruction instruction)
{
    cpu.m_nextPC = (cpu.m_nextPC & 0xF0000000) | (instruction.imm_jump() << 2); // TODO: move this shift to imm_jump()?
}

void InstructionSet::JAL(CPU& cpu, Instruction instruction)
{
    cpu.setReg(31, cpu.m_nextPC);
    J(cpu, instruction);
}

void InstructionSet::BEQ(CPU& cpu, Instruction instruction)
{
    uint32_t imm = instruction.imm_se();
    RegisterIndex s = instruction.regS();
    RegisterIndex t = instruction.regT();

    if (cpu.getReg(s) == cpu.getReg(t)) {
        cpu.m_nextPC += (imm << 2);
        cpu.m_nextPC -= 4;
    }
}

void InstructionSet::BNE(CPU& cpu, Instruction instruction)
{
    uint32_t imm = instruction.imm_se();
    RegisterIndex s = instruction.regS();
    RegisterIndex t = instruction.regT();

    if (cpu.getReg(s) != cpu.getReg(t)) {
        cpu.m_nextPC += (imm << 2);
        cpu.m_nextPC -= 4;
    }
}

void InstructionSet::BLEZ(CPU& cpu, Instruction instruction)
{
    uint32_t imm = instruction.imm_se();
    RegisterIndex s = instruction.regS();
    int32_t value = cpu.getReg(s);

    if (value <= 0) {
        cpu.m_nextPC += (imm << 2);
        cpu.m_nextPC -= 4;
    }
}

void InstructionSet::BGTZ(CPU& cpu, Instruction instruction)
{
    uint32_t imm = instruction.imm_se();
    RegisterIndex s = instruction.regS();
    int32_t value = cpu.getReg(s);

    if (value  > 0) {
        cpu.m_nextPC += (imm << 2);
        cpu.m_nextPC -= 4;
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

void InstructionSet::SLTI(CPU& cpu, Instruction instruction)
{
    int32_t imm = instruction.imm_se();
    RegisterIndex s = instruction.regS();
    RegisterIndex t = instruction.regT();

    uint32_t value = static_cast<int32_t>(cpu.getReg(s)) < imm;
    cpu.setReg(t, value);
}

void InstructionSet::SLTIU(CPU& cpu, Instruction instruction)
{
    uint32_t imm = instruction.imm_se();
    RegisterIndex s = instruction.regS();
    RegisterIndex t = instruction.regT();

    uint32_t value = cpu.getReg(s) < imm;
    cpu.setReg(t, value);
}

void InstructionSet::ANDI(CPU& cpu, Instruction instruction)
{
    RegisterIndex t = instruction.regT();
    RegisterIndex s = instruction.regS();
    uint32_t value = cpu.getReg(s) & instruction.imm();
    cpu.setReg(t, value);
}

void InstructionSet::ORI(CPU& cpu, Instruction instruction)
{
    RegisterIndex t = instruction.regT();
    RegisterIndex s = instruction.regS();
    uint32_t value = cpu.getReg(s) | instruction.imm();
    cpu.setReg(t, value);
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
    case 0b00000:
        MFC0(cpu, instruction);
        break;
    case 0b00100:
        MTC0(cpu, instruction);
        break;
    default:
        std::cerr << "Unhandled cop0 instruction: " << std::hex << instruction.copfn() << std::dec << '\n';
    }
}

void InstructionSet::MFC0(CPU& cpu, Instruction instruction)
{
    RegisterIndex cpuReg = instruction.regT();
    RegisterIndex copReg = instruction.regD();
    uint32_t value = 0xDEADBEEF;
    
    switch (copReg.index) {
    case 12:
        value = cpu.m_SR;
        break;
    case 13:
        value = cpu.m_CAUSE;
        break;
    case 14:
        value = cpu.m_EPC;
        break;
    default:
        std::cerr << "Unhandled MFC0 instruction\n";
    }

    cpu.m_pendingLoad = { cpuReg, value };
}

void InstructionSet::MTC0(CPU& cpu, Instruction instruction)
{
    // TODO: temp
    RegisterIndex cpuReg = instruction.regT();
    RegisterIndex copReg = instruction.regD();
    uint32_t value = cpu.getReg(cpuReg);

    switch (copReg.index) {
    case 3:
    case 5:
    case 6:
    case 7:
    case 9:
    case 11:
        if (value != 0)
            std::cerr << "Unhandled write to breakpoints registers\n";
        break; // ignore these if 0
    case 12:
        cpu.m_SR = value;
        break;
    case 13:
        if (value != 0)
            std::cerr << "Unhandled write to CAUSE register\n";
        break; // CAUSE register - ignore if 0
    default:
        std::cerr << "Unhandled MTC0 instruction\n";
    }
}

void InstructionSet::LB(CPU& cpu, Instruction instruction)
{
    if (cpu.m_SR & 0x10000)
        return; // isolated cache bit is set

    uint32_t imm = instruction.imm_se();
    RegisterIndex t = instruction.regT();
    RegisterIndex s = instruction.regS();
    uint32_t address = cpu.getReg(s) + imm;
    uint32_t value = static_cast<int8_t>(cpu.load8(address));
    cpu.m_pendingLoad = { t, value };
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
    cpu.m_pendingLoad = { t, value };
}

void InstructionSet::LBU(CPU& cpu, Instruction instruction)
{
    if (cpu.m_SR & 0x10000)
        return; // isolated cache bit is set

    uint32_t imm = instruction.imm_se();
    RegisterIndex t = instruction.regT();
    RegisterIndex s = instruction.regS();
    uint32_t address = cpu.getReg(s) + imm;
    uint32_t value = cpu.load8(address);
    cpu.m_pendingLoad = { t, value };
}

void InstructionSet::SB(CPU& cpu, Instruction instruction)
{
    if (cpu.m_SR & 0x10000)
        return; // isolated cache bit is set

    uint32_t imm = instruction.imm_se();
    RegisterIndex t = instruction.regT();
    RegisterIndex s = instruction.regS();
    uint32_t address = cpu.getReg(s) + imm;
    uint8_t value = cpu.getReg(t);
    cpu.store8(address, value);
}

void InstructionSet::SH(CPU& cpu, Instruction instruction)
{
    if (cpu.m_SR & 0x10000)
        return; // isolated cache bit is set

    uint32_t imm = instruction.imm_se();
    RegisterIndex t = instruction.regT();
    RegisterIndex s = instruction.regS();
    uint32_t address = cpu.getReg(s) + imm;
    uint16_t value = cpu.getReg(t);
    cpu.store16(address, value);
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
    zero,    // 0x00
    bxxxxx,  // 0x01
    J,       // 0x02
    JAL,     // 0x03
    BEQ,     // 0x04
    BNE,     // 0x05
    BLEZ,    // 0x06
    BGTZ,    // 0x07
    ADDI,    // 0x08
    ADDIU,   // 0x09
    SLTI,    // 0x0A
    SLTIU,   // 0x0B
    ANDI,    // 0x0C
    ORI,     // 0x0D
    invalid, // 0x0E
    LUI,     // 0x0F
    cop0,    // 0x10
    invalid, // 0x11
    invalid, // 0x12
    invalid, // 0x13
    invalid, // 0x14
    invalid, // 0x15
    invalid, // 0x16
    invalid, // 0x17
    invalid, // 0x18
    invalid, // 0x19
    invalid, // 0x1A
    invalid, // 0x1B
    invalid, // 0x1C
    invalid, // 0x1D
    invalid, // 0x1E
    invalid, // 0x1F
    LB,      // 0x20
    invalid, // 0x21
    invalid, // 0x22
    LW,      // 0x23
    LBU,     // 0x24
    invalid, // 0x25
    invalid, // 0x26
    invalid, // 0x27
    SB,      // 0x28
    SH,      // 0x29
    invalid, // 0x2A
    SW,      // 0x2B
    invalid, // 0x2C
    invalid, // 0x2D
    invalid, // 0x2E
    invalid, // 0x2F
    invalid, // 0x30
    invalid, // 0x31
    invalid, // 0x32
    invalid, // 0x33
    invalid, // 0x34
    invalid, // 0x35
    invalid, // 0x36
    invalid, // 0x37
    invalid, // 0x38
    invalid, // 0x39
    invalid, // 0x3A
    invalid, // 0x3B
    invalid, // 0x3C
    invalid, // 0x3D
    invalid, // 0x3E
    invalid  // 0x3F
};
