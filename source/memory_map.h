#pragma once
#include "bios.h"

class MemoryMap
{
public:
	MemoryMap() = default;

	uint32_t load32(uint32_t offset) const;
	void store32(uint32_t address, uint32_t value);
private:
	BIOS m_bios;
};
