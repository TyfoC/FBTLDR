#pragma once
#ifndef KERNEL
#define KERNEL

#include "user-config.h"
#include "gdt.h"
#include "pci.h"

#define KERNEL_BASE_SEG_VALUE			0x1000
#define KERNEL_BASE_OFF_VALUE			0x0000
#define KERNEL_GDT_ENTRIES_COUNT		3
#define KERNEL_GDT_CODE_SEG_INDEX		0x01
#define KERNEL_GDT_DATA_SEG_INDEX		0x02
#define KERNEL_GDT_CODE_SEG_OFF			0x08
#define KERNEL_GDT_DATA_SEG_OFF			0x10

typedef struct ATTRIB_PACKED KernelHeader {
	uint32_t	BaseAddress;
	uint32_t	FileLength;
	uint32_t	EntryPoint;
	uint32_t	StackTopAddress;
} KernelHeader;

typedef struct ATTRIB_PACKED KernelInitializationData {
	uint32_t	MemoryMapAddress;
	uint32_t	MemoryMapLength;
	uint32_t	GdtAddress;
	uint32_t	GdtEntriesCount;
	uint32_t	GdtCodeSegIndex;
	uint32_t	GdtDataSegIndex;
	uint32_t	PciSupportBits;
	UserConfig	UserKernelConfig;
} KernelInitializationData;

extern GdtRegister KernelGdtRegister;
extern GdtEntry KernelGdt[KERNEL_GDT_ENTRIES_COUNT];

#endif