#pragma once
#ifndef KERNEL
#define KERNEL

#include "terminal.h"
#include "gdt.h"

#define KERNEL_GDT_ENTRIES_COUNT	3

typedef struct ATTRIB(__packed__) KERNEL_INIT_DATA {
	UINT32		MemoryMapAddress;
	UINT32		MemoryMapLength;
	GDT_ENTRY*	GDTPointer;
	UINT32		GDTEntriesCount;
	UINT32		GDTCodeSegIndex;
	UINT32		GDTDataSegIndex;
} KERNEL_INIT_DATA;

extern GDT_REGISTER KernelGDTRegister;
extern GDT_ENTRY KernelGDT[KERNEL_GDT_ENTRIES_COUNT];

#endif