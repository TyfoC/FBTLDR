#pragma once
#ifndef MEMINFO
#define MEMINFO

#include "segoff.h"

typedef struct Ard {
	uint64_t	BaseAddress;
	uint64_t	Length;
	uint32_t	Type;
	uint32_t	ExtendedAttributes;
} Ard;

bool_t GetMemoryMap(size_t memoryMapSeg, size_t memoryMapOff, uint32_t* entriesCount);

#endif