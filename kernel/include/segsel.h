#pragma once
#ifndef SEGSEL
#define SEGSEL

#include "typedefs.h"

typedef struct ATTRIB(__packed__) {
	UINT8	RPL		:2;					//	requested privilege level
	UINT8	TI		:1;					//	table index (0 - GDT, 1 - LDT)
	UINT16	Index	:13;				//	entry index
} SEGMENT_SELECTOR;

VOID InitSegmentSelector(UINT16 value, SEGMENT_SELECTOR* segSel);

#endif