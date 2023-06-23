#pragma once
#ifndef GDT
#define GDT

#include "typedefs.h"

#define GDT_ENTRY_ACCESS_ACCESSED			0x01
#define GDT_ENTRY_ACCESS_READ_WRITE			0x02
#define GDT_ENTRY_ACCESS_DIRECTION			0x04
#define GDT_ENTRY_ACCESS_EXECUTABLE			0x08
#define GDT_ENTRY_ACCESS_CODE_DATA_SEGMENT	0x10
#define GDT_ENTRY_ACCESS_USER				0x60
#define GDT_ENTRY_ACCESS_PRESENT			0x80

#define GDT_ENTRY_FLAG_LONG_MODE_CODE		0x02
#define GDT_ENTRY_FLAG_SIZE					0x04
#define GDT_ENTRY_FLAG_GRANULARITY			0x08

typedef struct ATTRIB(__packed__) GDT_REGISTER {
	UINT16	Size;
	UINT32	Offset;
} GDT_REGISTER;

typedef struct ATTRIB(__packed__) GDT_ENTRY {
	UINT16	LimitLow;
	UINT32	BaseLow				:24;
	UINT8	Access;									//	GDT_ENTRY_ACCESS_*
	UINT8	LimitHigh			:4;
	UINT8	Flags				:4;					//	GDT_ENTRY_FLAG_*
	UINT8	BaseHigh;
} GDT_ENTRY;

VOID InitGDTRegister(const GDT_ENTRY* gdt, SIZE_T entriesCount, GDT_REGISTER* gdtReg);
VOID InitGDTEntry(UINT32 base, UINT32 limit, UINT8 access, UINT8 flags, GDT_ENTRY* gdtEntry);
VOID LoadGDTRegister(const GDT_REGISTER* gdtRegister);

#endif