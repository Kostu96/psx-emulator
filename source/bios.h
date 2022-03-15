#pragma once
#include "type_helpers.h"

#include <cstdint>

class BIOS
{
public:
	static const AddressRange RANGE;

	BIOS();
	~BIOS();

	uint32_t load32(uint32_t offset) const;
private:
	static const size_t SIZE = 512 * 1024;
	static const char* FILENAME;
	uint8_t* m_data;
};
