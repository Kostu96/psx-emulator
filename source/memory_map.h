#pragma once
#include "bios.h"

class MemoryMap
{
public:
	MemoryMap() = default;

	uint32_t load32(uint32_t offset) const;
	void store32(uint32_t address, uint32_t value);
private:
	static const AddressRange MEMORY_CONTROL_RANGE;
	static const AddressRange RAM_SIZE_RANGE;
	BIOS m_bios;
};
