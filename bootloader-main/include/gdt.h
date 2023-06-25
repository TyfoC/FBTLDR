#pragma once
#ifndef GDT
#define GDT

#include "typedefs.h"

#define GDT_ACCESS_ACCESSED		0x01
#define GDT_ACCESS_READ_WRITE	0x02
#define GDT_ACCESS_DIRECTION	0x04
#define GDT_ACCESS_EXECUTABLE	0x08
#define GDT_ACCESS_CODE_OR_DATA	0x10
#define GDT_ACCESS_KERNEL		0x00
#define GDT_ACCESS_USER			0x60
#define GDT_ACCESS_PRESENT		0x80

#define GDT_FLAG_RESERVED		0x01
#define GDT_FLAG_LONG_MODE		0x02
#define GDT_FLAG_PROTECTED_MODE	0x04
#define GDT_FLAG_4KB_PAGES		0x08

typedef struct ATTRIB_PACKED {
	uint16_t	Size;
	uint32_t	Offset;
} GdtRegister;

typedef struct ATTRIB_PACKED {
	uint16_t	LimitLow;
	uint16_t	BaseLow;
	uint8_t		BaseMiddle;
	uint8_t		AccessByte;
	uint8_t		LimitHighAndFlags;
	uint8_t		BaseHigh;
} GdtEntry;

GdtRegister CreateGdtRegister(const GdtEntry* gdtEntries, size_t count);
GdtEntry CreateGdtEntry(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);

#endif