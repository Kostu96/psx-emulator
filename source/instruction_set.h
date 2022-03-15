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
	static void ORI(CPU& cpu, Instruction instruction);
	static void LUI(CPU& cpu, Instruction instruction);
	static void SW(CPU& cpu, Instruction instruction);
};
