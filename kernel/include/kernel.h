#pragma once
#ifndef KERNEL
#define KERNEL

#include "terminal.h"
#include "pic.h"
#include "fpu.h"
#include "pit.h"
#include "pci.h"
#include "acpi.h"
#include "ps2-ctrl.h"
#include "ps2-kbd.h"
#include "ps2-mouse.h"
#include "ide.h"

#define KERNEL_BASE_ADDRESS							((SIZE_T)&__PTR_KERNEL_BASE_ADDRESS__)
#define KERNEL_END_ADDRESS							((SIZE_T)&__PTR_KERNEL_END_ADDRESS__)
#define KERNEL_SIZE									(KERNEL_END_ADDRESS - KERNEL_BASE_ADDRESS)

#define KERNEL_GDT_ENTRIES_COUNT					3
#define KERNEL_IDT_ENTRIES_COUNT					256

#define PCI_SUPPORT_FIRST_CONFIG_ACCESS_MECHANISM	0x01
#define PCI_SUPPORT_SECOND_CONFIG_ACCESS_MECHANISM	0x02
#define PCI_SUPPORT_CHECK_COMPLETED_SUCCESSFULLY	0x04

typedef struct ATTRIB_PACKED KERNEL_INIT_DATA {
	UINT32		MemoryMapAddress;
	UINT32		MemoryMapLength;
	GDT_ENTRY*	GDTPointer;
	UINT32		GDTEntriesCount;
	UINT32		GDTCodeSegIndex;
	UINT32		GDTDataSegIndex;
	UINT32		PCISupportBits;
} KERNEL_INIT_DATA;

extern SIZE_T __PTR_KERNEL_BASE_ADDRESS__;
extern SIZE_T __PTR_KERNEL_END_ADDRESS__;

VOID ShutdownKernel(SIZE_T secondsCount);

extern GDT_REGISTER ATTRIB(__aligned__(16)) KernelGDTRegister;
extern GDT_ENTRY ATTRIB(__aligned__(16)) KernelGDT[KERNEL_GDT_ENTRIES_COUNT];
extern IDT_REGISTER ATTRIB(__aligned__(16)) KernelIDTRegister;
extern IDT_ENTRY ATTRIB(__aligned__(16)) KernelIDT[KERNEL_IDT_ENTRIES_COUNT];
extern UINT16 KernelCodeSegValue;
extern UINT16 KernelDataSegValue;

#endif