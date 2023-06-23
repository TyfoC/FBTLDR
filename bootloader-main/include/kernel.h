#pragma once
#ifndef KERNEL
#define KERNEL

#include "gdt.h"

#define KERNEL_BASE_SEG_VALUE			0x1000
#define KERNEL_BASE_OFF_VALUE			0x0000
#define KERNEL_GDT_ENTRIES_COUNT		3
#define KERNEL_GDT_CODE_SEG_INDEX		0x01
#define KERNEL_GDT_DATA_SEG_INDEX		0x02
#define KERNEL_GDT_CODE_SEG_OFF			0x08
#define KERNEL_GDT_DATA_SEG_OFF			0x10

typedef struct ATTRIB(__packed__) KernelHeader {
	uint32_t	BaseAddress;
	uint32_t	FileLength;
	uint32_t	EntryPoint;
	uint32_t	StackTopAddress;
} KernelHeader;

typedef struct ATTRIB(__packed__) KernelInitializationData {
	uint32_t	MemoryMapAddress;
	uint32_t	MemoryMapLength;
} KernelInitializationData;

extern GdtRegister KernelGdtRegister;
extern GdtEntry KernelGdt[KERNEL_GDT_ENTRIES_COUNT];

#endif