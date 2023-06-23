#pragma once
#ifndef KERNEL
#define KERNEL

#include "terminal.h"

typedef struct ATTRIB(__packed__) KERNEL_INIT_DATA {
	UINT32	MemoryMapAddress;
	UINT32	MemoryMapLength;
	UINT32	GdtAddress;
	UINT32	GdtEntriesCount;
	UINT32	GdtCodeSegIndex;
	UINT32	GdtDataSegIndex;
} KERNEL_INIT_DATA;

#endif