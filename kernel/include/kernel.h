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

#define VIDEO_MODE_TYPE_BIOS						0x00
#define VIDEO_MODE_TYPE_SVGA_TEXT					0x01
#define VIDEO_MODE_TYPE_SVGA_GRAPHICS				0x02

#define KERNEL_FILE_TYPE_ELF						0x00
#define KERNEL_FILE_TYPE_EXE						0x01

typedef struct ATTRIB_PACKED KERNEL_INIT_DATA {
	UINT32		MemoryMapAddress;
	UINT32		MemoryMapLength;
	GDT_ENTRY*	GDTPointer;
	UINT32		GDTEntriesCount;
	UINT32		GDTCodeSegIndex;
	UINT32		GDTDataSegIndex;
	UINT32		PCISupportBits;
	UINT16		VideoModeType;					//	VIDEO_MODE_TYPE_*
	UINT16		VideoModeIndex;
	UINT16		VideoModeWidth;
	UINT16		VideoModeHeight;
	UINT16		VideoModeBPP;
	UINT16		VideoModeFuzzy;
	UINT16		KernelFileType;					//	KERNEL_FILE_TYPE_
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