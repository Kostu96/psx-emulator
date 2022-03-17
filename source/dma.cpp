#include "dma.h"

#include <iostream>

uint32_t DMA::load32(uint32_t offset) const
{
	switch (offset) {
	case 0x70: return m_control;
	}

	std::cerr << "Unhandled load from DMA: " << offset << '\n';
	return 0xDEADBEEF;
}

void DMA::store32(uint32_t offset, uint32_t value)
{
	switch (offset) {
	case 0x70:
		m_control = value;
		return;
	}

	std::cerr << "Unhandled write to DMA: " << offset << '\n';
}
