#pragma once
#ifndef MEMORY
#define MEMORY

#include "typedefs.h"

#define ARD_REGION_EXTENDED_ATTRIBUTES_PRESENT		0x01

#define MEMORY_REGION_TYPE_USABLE					0x01
#define MEMORY_REGION_TYPE_RESERVED					0x02
#define MEMORY_REGION_TYPE_RECLAIMABLE				0x03

#define MEMORY_REGION_OVERLAP_NONE					0x00
#define MEMORY_REGION_OVERLAP_START					0x01
#define MEMORY_REGION_OVERLAP_END					0x02
#define MEMORY_REGION_OVERLAP_INSIDE				0x03
#define MEMORY_REGION_OVERLAP_OUTSIDE				0x04

#define MEMORY_FRAME_SIZE							0x1000
#define MEMORY_FRAME_FLAG_BUSY						0x01

typedef struct ATTRIB(__packed__) {
	UINT64	BaseAddress;
	UINT64	Length;
	UINT32	Type;
	UINT32	ExtendedAttributes;
} ADDRESS_RANGE_DESCRIPTOR;

typedef struct ATTRIB(__packed__) {
	SIZE_T	Address;					//	address of MEMORY_FRAME_DESCRIPTOR[]
	SIZE_T	Size;						//	memory region size in bytes
	SIZE_T	Type;
	SIZE_T	Length;						//	count of MEMORY_FRAME_DESCRIPTOR
} MEMORY_REGION_DESCRIPTOR;

typedef struct ATTRIB(__packed__) {
	SIZE_T	DataOffset;					//	data offset from this MEMORY_FRAME_DESCRIPTOR
	SIZE_T	BusyFramesCount;
	SIZE_T	Flags;						//	MEMORY_FRAME_FLAG_*
} MEMORY_FRAME_DESCRIPTOR;

typedef struct ATTRIB(__packed__) {
	SIZE_T	DataAddress;
	SIZE_T	DataSize;
	SIZE_T	RegionIndex;
	SIZE_T	FrameIndex;
} MEMORY_ALLOCATION_DATA;

VOID InitPMM(MEMORY_REGION_DESCRIPTOR* regs, SIZE_T count);
SIZE_T TransformBIOSMemoryMap(const ADDRESS_RANGE_DESCRIPTOR* ard, MEMORY_REGION_DESCRIPTOR* mrd, SIZE_T count);
UINT8 GetMemoryRegionOverlapType(const MEMORY_REGION_DESCRIPTOR* reg, const MEMORY_REGION_DESCRIPTOR* overlappingReg);
SIZE_T RemoveMemoryRegions(MEMORY_REGION_DESCRIPTOR* regs, const MEMORY_REGION_DESCRIPTOR* rmRegs, SIZE_T regsCnt, SIZE_T rmRegsCnt);
SIZE_T GetMemoryMapLength(VOID);
VOID GetMemoryRegion(SIZE_T index, MEMORY_REGION_DESCRIPTOR* mrd);

VOID* AllocatePhysicalMemory(SIZE_T count);
VOID* ReallocatePhysicalMemory(VOID* memory, SIZE_T count);
SIZE_T FreePhysicalMemory(VOID* memory);
MEMORY_ALLOCATION_DATA AllocatePhysicalFrames(SIZE_T bytesCount);
MEMORY_ALLOCATION_DATA ReallocatePhysicalFrames(MEMORY_ALLOCATION_DATA* memAllocData, SIZE_T bytesCount);
SIZE_T FreePhysicalFrames(MEMORY_ALLOCATION_DATA* memAllocData);
VOID CopyMemory(VOID* destination, const VOID* source, SIZE_T count);
VOID FillMemory(VOID* destination, UINT8 value, SIZE_T count);

#endif