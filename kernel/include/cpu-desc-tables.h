#pragma once
#ifndef CPU_DESC_TABLES
#define CPU_DESC_TABLES

#include "segsel.h"

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

#define IDT_GATE_TYPE_TASK					0x05
#define IDT_GATE_TYPE_INT16					0x06
#define IDT_GATE_TYPE_TRAP16				0x07
#define IDT_GATE_TYPE_INT32					0x0E
#define IDT_GATE_TYPE_TRAP32				0x0F

#define IDT_FLAG_USER						0x06
#define IDT_FLAG_PRESENT					0x08

#define MAX_IDT_ENTRIES_COUNT				0x0100

typedef struct ATTRIB_PACKED GDT_REGISTER {
	UINT16	Size;
	UINT32	Offset;
} GDT_REGISTER;

typedef struct ATTRIB_PACKED GDT_ENTRY {
	UINT16	LimitLow;
	UINT32	BaseLow							:24;
	UINT8	Access;												//	GDT_ENTRY_ACCESS_*
	UINT8	LimitHigh						:4;
	UINT8	Flags							:4;					//	GDT_ENTRY_FLAG_*
	UINT8	BaseHigh;
} GDT_ENTRY;

typedef struct ATTRIB_PACKED IDT_REGISTER {
	UINT16	Size;
	UINT32	Offset;
} IDT_REGISTER;

typedef struct ATTRIB_PACKED IDT_ENTRY {
	UINT16				BaseLow;
	SEGMENT_SELECTOR	CodeSegSelector;
	UINT8				Reserved;
	UINT8				GateType			:4;					//	IDT_GATE_TYPE_*
	UINT8				Flags				:4;					//	IDT_FLAG_*
	UINT16				BaseHigh;
} IDT_ENTRY;

VOID InitGDTRegister(const GDT_ENTRY* gdt, SIZE_T entriesCount, GDT_REGISTER* gdtReg);
VOID InitGDTEntry(UINT32 base, UINT32 limit, UINT8 access, UINT8 flags, GDT_ENTRY* gdtEntry);
VOID InitIDTRegister(const IDT_ENTRY* idt, SIZE_T entriesCount, IDT_REGISTER* idtReg);
VOID InitIDTEntry(UINT32 base, SEGMENT_SELECTOR codeSegSelector, UINT8 gateType, UINT8 flags, IDT_ENTRY* idtEntry);

#endif