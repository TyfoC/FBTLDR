#include <segoff.h>

void AddValueToSegOff(size_t* segValue, size_t* offValue, size_t value) {
	if (*offValue - 1 >= 0xffff - value) ++(*segValue);
	*offValue += value;
}