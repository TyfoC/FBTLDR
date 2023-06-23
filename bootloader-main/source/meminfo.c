#include <meminfo.h>

size_t GetMemoryMapElement(size_t elementSeg, size_t elementOff, uint32_t prevOffset);

bool_t GetMemoryMap(size_t memoryMapSeg, size_t memoryMapOff, uint32_t* entriesCount) {
	uint32_t prevOffset = GetMemoryMapElement(memoryMapSeg, memoryMapOff, 0);
	if (!prevOffset) return FALSE;
	
	++(*entriesCount);
	AddValueToSegOff(&memoryMapSeg, &memoryMapOff, sizeof(Ard));

	while (TRUE) {
		prevOffset = GetMemoryMapElement(memoryMapSeg, memoryMapOff, prevOffset);
		++(*entriesCount);
		if (!prevOffset) break;
		AddValueToSegOff(&memoryMapSeg, &memoryMapOff, sizeof(Ard));
	}

	return TRUE;
}