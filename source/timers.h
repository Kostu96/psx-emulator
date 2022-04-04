#pragma once
#include <cpp-common/non_copyable.h>
#include <cstdint>

class Timers :
	public NonCopyable
{
public:
	Timers() = default;

	uint32_t load32(uint32_t offset) const;
	void store16(uint32_t offset, uint16_t value);
	void store32(uint32_t offset, uint32_t value);
private:
	struct CounterMode {
		uint32_t syncEnable : 1; // 0
		uint32_t syncMode   : 2; // 1-2
		uint32_t resetMode  : 1; // 3
	};

	struct Counter {
		uint16_t value;
		uint16_t notUsed;
	};
};
