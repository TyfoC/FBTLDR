#include <segsel.h>

VOID InitSegmentSelector(UINT16 value, SEGMENT_SELECTOR* segSel) {
	segSel->RPL = value & 0b11;
	segSel->TI = (value >> 2) & 0b1;
	segSel->Index = value >> 3;
}