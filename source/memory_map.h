#pragma once
#include "bios.h"
#include "ram.h"

class MemoryMap
{
public:
	MemoryMap() = default;

	uint32_t load32(uint32_t offset) const;
	void store8(uint32_t address, uint16_t value);
	void store16(uint32_t address, uint16_t value);
	void store32(uint32_t address, uint32_t value);
private:
	static uint32_t maskRegion(uint32_t address);

	static const uint32_t REGION_MASK[];
	static const AddressRange RAM_RANGE;
	static const AddressRange BIOS_RANGE;
	static const AddressRange SYS_CONTROL_RANGE;
	static const AddressRange SPU_RANGE;
	static const AddressRange RAM_SIZE_RANGE;
	static const AddressRange CACHE_CONTROL_RANGE;
	static const AddressRange EXPANSION2_RANGE;
	
	BIOS m_bios;
	RAM m_ram;
};
