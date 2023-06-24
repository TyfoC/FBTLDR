#pragma once
#ifndef KERNEL
#define KERNEL

#include "asm-utils.h"
#include "cpu-desc-tables.h"
#include "cpu-int-handlers.h"
#include "terminal.h"

#define KERNEL_GDT_ENTRIES_COUNT				3
#define KERNEL_IDT_ENTRIES_COUNT				256

typedef struct ATTRIB(__packed__) KERNEL_INIT_DATA {
	UINT32		MemoryMapAddress;
	UINT32		MemoryMapLength;
	GDT_ENTRY*	GDTPointer;
	UINT32		GDTEntriesCount;
	UINT32		GDTCodeSegIndex;
	UINT32		GDTDataSegIndex;
} KERNEL_INIT_DATA;

extern GDT_REGISTER ATTRIB(__aligned__(16)) KernelGDTRegister;
extern GDT_ENTRY ATTRIB(__aligned__(16)) KernelGDT[KERNEL_GDT_ENTRIES_COUNT];
extern IDT_REGISTER ATTRIB(__aligned__(16)) KernelIDTRegister;
extern IDT_ENTRY ATTRIB(__aligned__(16)) KernelIDT[KERNEL_IDT_ENTRIES_COUNT];
extern UINT16 KernelCodeSegValue;
extern UINT16 KernelDataSegValue;

#endif