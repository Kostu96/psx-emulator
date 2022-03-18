#pragma once
#include "type_helpers.h"

#include <cstdint>

class GPU
{
public:
	GPU() = default;

	uint32_t load32(uint32_t offset) const;
	void store32(uint32_t offset, uint32_t value);
private:
	void gp0write(uint32_t value);
	void gp1write(uint32_t value);

	union {
		struct {
			uint32_t pageBaseX             : 4; // 0-3
			uint32_t pageBaseY             : 1; // 4
			uint32_t semiTransparency      : 2; // 5-6
			uint32_t pageColorDepth        : 2; // 7-8
			uint32_t dithering             : 1; // 9
			uint32_t drawToDisplay         : 1; // 10
			uint32_t forceSetMask          : 1; // 11
			uint32_t preserveMasked        : 1; // 12
			uint32_t field                 : 1; // 13
			uint32_t reverse               : 1; // 14
			uint32_t textureDisable        : 1; // 15
			uint32_t horizontalResolution2 : 1; // 16
			uint32_t horizontalResolution1 : 2; // 17-18
			uint32_t verticalResolution    : 1; // 19
			uint32_t videoMode             : 1; // 20
			uint32_t displayColorDepth     : 1; // 21
			uint32_t interlace             : 1; // 22
			uint32_t displayDisable        : 1; // 23
			uint32_t interrupt             : 1; // 24
			uint32_t DMAorDaraRequest      : 1; // 25
			uint32_t receiveCMDReady       : 1; // 26
			uint32_t sendVRAMReady         : 1; // 27
			uint32_t receiveDMAReady       : 1; // 28
			uint32_t DMADirection          : 2; // 29-30
			uint32_t drawOddInInterlaced   : 1; // 31
		} fields;
		uint32_t word = 0x800000; // all 0s except displayDisable
	} m_status;
};
