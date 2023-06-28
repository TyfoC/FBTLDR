#include <segoff.h>

void AddValueToSegOff(size_t* segValue, size_t* offValue, size_t value) {
	if (*offValue >= (uint16_t)(0x10000 - value)) ++(*segValue);
	*offValue += value;
}