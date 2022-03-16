#pragma once
#include "type_helpers.h"

class CPU;

class InstructionSet
{
public:
	using InstFuncType = void (*)(CPU&, Instruction);
	static InstFuncType Fns[];
private:
	static void invalid(CPU& cpu, Instruction instruction);
	static void zero(CPU& cpu, Instruction instruction);
	static void SSL(CPU& cpu, Instruction instruction);
	static void OR(CPU& cpu, Instruction instruction);
	static void J(CPU& cpu, Instruction instruction);
	static void BNE(CPU& cpu, Instruction instruction);
	static void ADDI(CPU& cpu, Instruction instruction);
	static void ADDIU(CPU& cpu, Instruction instruction);
	static void ORI(CPU& cpu, Instruction instruction);
	static void LUI(CPU& cpu, Instruction instruction);
	static void cop0(CPU& cpu, Instruction instruction);
	static void MTC0(CPU& cpu, Instruction instruction);
	static void LW(CPU& cpu, Instruction instruction);
	static void SW(CPU& cpu, Instruction instruction);
};
