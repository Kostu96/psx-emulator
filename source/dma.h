#pragma once
#include <cstdint>

class DMA
{
public:
	DMA() = default;

	uint32_t load32(uint32_t offset) const;
	void store32(uint32_t offset, uint32_t value);
private:
	// TODO: make this a bitfield
	uint32_t m_control = 0x07654321;
	uint8_t m_channelIRQEnable = 0;
	uint8_t m_channelIRQFlags = 0;
	uint8_t m_IRQDummy = 0;
	bool m_IRQEnable = false;
	bool m_forceIRQ = false;
};
