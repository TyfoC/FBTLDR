#include <memory.h>

static MEMORY_REGION_DESCRIPTOR* Regions;
static SIZE_T NumberOfRegions = 0;

static MEMORY_ALLOCATION_DATA* Allocations = 0;
static SIZE_T NumberOfAllocations = 0;
static MEMORY_ALLOCATION_DATA AllocData = { 0, 0, NPOS, NPOS };

VOID InitPMM(MEMORY_REGION_DESCRIPTOR* regs, SIZE_T count) {
	MEMORY_REGION_DESCRIPTOR memMapReg = {
		(SIZE_T)regs, sizeof(MEMORY_REGION_DESCRIPTOR) * (count + 2), MEMORY_REGION_TYPE_RESERVED, 0
	};

	Regions = regs;
	NumberOfRegions = RemoveMemoryRegions(Regions, &memMapReg, count, 1);

	SIZE_T dataAddr, j;
	MEMORY_FRAME_DESCRIPTOR* frameDescr;
	for (SIZE_T i = 0; i < NumberOfRegions; i++) {
		if (Regions[i].Type == MEMORY_REGION_TYPE_USABLE || Regions[i].Type == MEMORY_REGION_TYPE_RECLAIMABLE) {
			frameDescr = (MEMORY_FRAME_DESCRIPTOR*)(((SIZE_T)Regions[i].Address));
			dataAddr = Regions[i].Address + ALIGN_UP(Regions[i].Length * sizeof(MEMORY_FRAME_DESCRIPTOR), MEMORY_FRAME_SIZE);

			for (j = 0; j < Regions[i].Length; j++) {
				frameDescr->DataOffset = dataAddr - (SIZE_T)frameDescr;
				frameDescr->BusyFramesCount = 0;
				frameDescr->Flags = 0;
				++frameDescr;
				dataAddr += MEMORY_FRAME_SIZE;
			}
		}
	}

	AllocData.DataAddress = (SIZE_T)Allocations;
}

SIZE_T TransformBIOSMemoryMap(const ADDRESS_RANGE_DESCRIPTOR* ard, MEMORY_REGION_DESCRIPTOR* mrd, SIZE_T count) {
	SIZE_T mrdLength = 0, ardType, mrdAddr, mrdSize, i;
	UINT64 ardAddr, ardLen;
	for (i = 0; i < count; i++) {
		ardAddr = ard[i].BaseAddress;
		ardLen = ard[i].Length;
		ardType = (
			ard[i].ExtendedAttributes & ARD_REGION_EXTENDED_ATTRIBUTES_PRESENT ? ard[i].Type : MEMORY_REGION_TYPE_RESERVED
		);

		if (ardAddr >= 0x0000000100000000) continue;
		else if (ardAddr + ardLen > 0x0000000100000000) ardLen = 0x0000000100000000 - ardAddr;

		mrdAddr = ALIGN_UP((SIZE_T)(ardAddr & 0xFFFFFFFF), MEMORY_FRAME_SIZE);
		mrdSize = ALIGN_DOWN((SIZE_T)(ardLen & 0xFFFFFFFF), MEMORY_FRAME_SIZE);

		if (!mrdSize) continue;

		mrd[mrdLength].Address = mrdAddr;
		mrd[mrdLength].Size = mrdSize;
		mrd[mrdLength].Length = mrdSize / (sizeof(MEMORY_FRAME_DESCRIPTOR) + MEMORY_FRAME_SIZE);
		mrd[mrdLength].Type = ardType;
		++mrdLength;
	}

	if (!mrdLength) return 0;

	//	bubble sort
	SIZE_T j;
	BOOL swapped;
	MEMORY_REGION_DESCRIPTOR tmpRegDescr;
	for (i = 0; i < mrdLength - 1; i++) {
		swapped = FALSE;
		for (j = 0; j < mrdLength - i - 1; j++) {
			if (mrd[j].Address >= mrd[j + 1].Address) {
				CopyMemory(&tmpRegDescr, &mrd[j], sizeof(MEMORY_REGION_DESCRIPTOR));
				CopyMemory(&mrd[j], &mrd[j + 1], sizeof(MEMORY_REGION_DESCRIPTOR));
				CopyMemory(&mrd[j + 1], &tmpRegDescr, sizeof(MEMORY_REGION_DESCRIPTOR));
				swapped = TRUE;
			}
		}

		if (!swapped) break;
	}

	return mrdLength;
}

UINT8 GetMemoryRegionOverlapType(const MEMORY_REGION_DESCRIPTOR* reg, const MEMORY_REGION_DESCRIPTOR* overlappingReg) {
	SIZE_T regStart = reg->Address;
	SIZE_T regEnd = regStart + reg->Size;
	SIZE_T ovlStart = overlappingReg->Address;
	SIZE_T ovlEnd = ovlStart + overlappingReg->Size;

	if (ovlStart <= regStart && ovlEnd > regStart && ovlEnd < regEnd ) return MEMORY_REGION_OVERLAP_START;
	else if (ovlStart < regEnd && ovlStart > regStart && ovlEnd >= regEnd) return MEMORY_REGION_OVERLAP_END;
	else if (ovlStart >= regStart && ovlEnd <= regEnd) return MEMORY_REGION_OVERLAP_INSIDE;
	else if (ovlStart <= regStart && ovlEnd >= regEnd) return MEMORY_REGION_OVERLAP_OUTSIDE;

	return MEMORY_REGION_OVERLAP_NONE;
}

SIZE_T RemoveMemoryRegions(MEMORY_REGION_DESCRIPTOR* regs, const MEMORY_REGION_DESCRIPTOR* rmRegs, SIZE_T regsCnt, SIZE_T rmRegsCnt) {
	SIZE_T rgStart, rgSize, rgEnd, rmRgStart, rmRgSize, rmRgEnd;
	UINT8 overlapType;

	for (SIZE_T i = 0; i < regsCnt; i++) {
		if (regs[i].Type != MEMORY_REGION_TYPE_USABLE && regs[i].Type != MEMORY_REGION_TYPE_RECLAIMABLE) continue;
		for (SIZE_T j = 0; j < rmRegsCnt; j++) {
			if (!rmRegs[j].Size) continue;
			overlapType = GetMemoryRegionOverlapType(&regs[i], &rmRegs[j]);

			rgStart = regs[i].Address;
			rgSize = regs[i].Size;
			rgEnd = rgStart + rgSize;

			rmRgStart = rmRegs[j].Address;
			rmRgSize = rmRegs[j].Size;
			rmRgEnd = rmRgStart + rmRgSize;

			if (overlapType == MEMORY_REGION_OVERLAP_START) {
				regs[i].Size -= rmRgEnd - rgStart;
				regs[i].Address = rmRgEnd;
			}
			else if (overlapType == MEMORY_REGION_OVERLAP_END) regs[i].Size = rmRgStart - rgStart;
			else if (overlapType == MEMORY_REGION_OVERLAP_INSIDE) {
				for (SIZE_T k = regsCnt; k > i; k--) regs[k] = regs[k - 1];
				regs[i + 1].Address = rmRgEnd;
				regs[i + 1].Size = rgEnd - rmRgEnd;
				regs[i + 1].Type = regs[i].Type;
				regs[i].Size = rmRgStart - rgStart;
				regsCnt += 1;
			}
			else if (overlapType == MEMORY_REGION_OVERLAP_OUTSIDE) regs[i].Type = MEMORY_REGION_TYPE_RESERVED;
		}
	}

	//	align address & size for each entry
	for (SIZE_T i = 0; i < regsCnt; i++) {
		regs[i].Address = ALIGN_UP(regs[i].Address, MEMORY_FRAME_SIZE);
		regs[i].Size = ALIGN_DOWN(regs[i].Size, MEMORY_FRAME_SIZE);
		regs[i].Length = regs[i].Size / (sizeof(MEMORY_FRAME_DESCRIPTOR) + MEMORY_FRAME_SIZE);
	}

	return regsCnt;
}

SIZE_T GetMemoryMapLength(VOID) {
	return NumberOfRegions;
}

VOID GetMemoryRegion(SIZE_T index, MEMORY_REGION_DESCRIPTOR* mrd) {
	*mrd = index >= NumberOfRegions ? (MEMORY_REGION_DESCRIPTOR){} : Regions[index];
}

VOID* AllocatePhysicalMemory(SIZE_T count) {
	MEMORY_ALLOCATION_DATA allocData = AllocatePhysicalFrames(count);
	if (allocData.RegionIndex == NPOS && allocData.FrameIndex == NPOS) return 0;

	SIZE_T unalignedSize = sizeof(Allocations[0]) * NumberOfAllocations;
	SIZE_T curSize = ALIGN_UP(unalignedSize, MEMORY_FRAME_SIZE);
	SIZE_T nextSize = ALIGN_UP(unalignedSize + sizeof(Allocations[0]), MEMORY_FRAME_SIZE);
	if (nextSize > curSize) {
		MEMORY_ALLOCATION_DATA tmp = ReallocatePhysicalFrames(&AllocData, nextSize);
		if (tmp.RegionIndex == NPOS && tmp.FrameIndex == NPOS) {
			FreePhysicalFrames(&allocData);
			return 0;
		}

		AllocData = tmp;
		Allocations = (MEMORY_ALLOCATION_DATA*)tmp.DataAddress;
	}

	Allocations[NumberOfAllocations] = allocData;
	++NumberOfAllocations;

	return (VOID*)allocData.DataAddress;
}

VOID* ReallocatePhysicalMemory(VOID* memory, SIZE_T count) {
	VOID* alcMem = AllocatePhysicalMemory(count);
	if (!alcMem) return 0;

	if (memory) {
		CopyMemory(alcMem, memory, count);
		FreePhysicalMemory(memory);
	}

	return alcMem;
}

SIZE_T FreePhysicalMemory(VOID* memory) {
	MEMORY_ALLOCATION_DATA tmpAlcData;
	SIZE_T unalignedSize, curSize, nextSize;

	for (SIZE_T i = 0; i < NumberOfAllocations; i++) {
		if (Allocations[i].DataAddress == (SIZE_T)memory) {
			tmpAlcData = Allocations[i];
			if (NumberOfAllocations - 1) CopyMemory(
				&Allocations[i], &Allocations[i + 1],
				sizeof(MEMORY_ALLOCATION_DATA) * (NumberOfAllocations - i - 1)
			);

			unalignedSize = sizeof(Allocations[0]) * NumberOfAllocations;
			curSize = ALIGN_UP(unalignedSize, MEMORY_FRAME_SIZE);
			nextSize = ALIGN_UP(unalignedSize - sizeof(Allocations[0]), MEMORY_FRAME_SIZE);

			if (nextSize > curSize) {
				MEMORY_ALLOCATION_DATA tmp = ReallocatePhysicalFrames(&AllocData, nextSize);
				if (tmp.RegionIndex == NPOS && tmp.FrameIndex == NPOS) break;

				AllocData = tmp;
				Allocations = (MEMORY_ALLOCATION_DATA*)tmp.DataAddress;
			}

			NumberOfAllocations -= 1;
			return FreePhysicalFrames(&tmpAlcData);
		}
	}

	return 0;
}

MEMORY_ALLOCATION_DATA AllocatePhysicalFrames(SIZE_T bytesCount) {
	MEMORY_ALLOCATION_DATA result = { NPOS, 0, NPOS, NPOS };

	bytesCount = ALIGN_UP(bytesCount, MEMORY_FRAME_SIZE);
	SIZE_T j, k, pagesCount = bytesCount / MEMORY_FRAME_SIZE;
	BOOL framesFree;
	MEMORY_FRAME_DESCRIPTOR* frameDescriptors;

	for (SIZE_T i = 0; i < NumberOfRegions; i++) {
		if (Regions[i].Type != MEMORY_REGION_TYPE_USABLE && Regions[i].Type != MEMORY_REGION_TYPE_RECLAIMABLE) continue;

		frameDescriptors = (MEMORY_FRAME_DESCRIPTOR*)Regions[i].Address;

		if (pagesCount <= Regions[i].Length) {
			for (j = 0; j + pagesCount <= Regions[i].Length; j++) {
				framesFree = TRUE;

				for (k = j; k < j + pagesCount; k++) {
					if (frameDescriptors[k].Flags & MEMORY_FRAME_FLAG_BUSY) {
						framesFree = FALSE;
						break;
					}
				}

				if (framesFree) {
					frameDescriptors[j].BusyFramesCount = pagesCount;
					for (k = j; k < j + pagesCount; k++)frameDescriptors[k].Flags |= MEMORY_FRAME_FLAG_BUSY;

					result.DataAddress = (SIZE_T)&frameDescriptors[j] + frameDescriptors[j].DataOffset;
					result.DataSize = bytesCount;
					result.RegionIndex = i;
					result.FrameIndex = j;

					return result;
				}
			}
		}
	}

	return result;
}

MEMORY_ALLOCATION_DATA ReallocatePhysicalFrames(MEMORY_ALLOCATION_DATA* memAllocData, SIZE_T bytesCount) {
	MEMORY_ALLOCATION_DATA nAllocData = AllocatePhysicalFrames(bytesCount);
	if (nAllocData.RegionIndex == NPOS && nAllocData.FrameIndex == NPOS) return nAllocData;

	if (memAllocData->RegionIndex != NPOS || memAllocData->FrameIndex != NPOS) {
		CopyMemory((VOID*)nAllocData.DataAddress, (VOID*)memAllocData->DataAddress, memAllocData->DataSize);
		if (!FreePhysicalFrames(memAllocData)) {
			FreePhysicalFrames(&nAllocData);
			return (MEMORY_ALLOCATION_DATA){ NPOS, 0, NPOS, NPOS };
		}
	}

	return nAllocData;
}

SIZE_T FreePhysicalFrames(MEMORY_ALLOCATION_DATA* memAllocData) {
	if (memAllocData->RegionIndex == NPOS && memAllocData->FrameIndex == NPOS) return 0;

	MEMORY_FRAME_DESCRIPTOR* frameDescr = &(
		(MEMORY_FRAME_DESCRIPTOR*)Regions[memAllocData->RegionIndex].Address
	)[memAllocData->FrameIndex];

	SIZE_T framesCnt = frameDescr->BusyFramesCount;
	frameDescr->BusyFramesCount = 0;

	for (SIZE_T i = 0; i < framesCnt; i++) {
		frameDescr->Flags &= ~MEMORY_FRAME_FLAG_BUSY;
		++frameDescr;
	}

	return framesCnt;
}

VOID CopyMemory(VOID* destination, const VOID* source, SIZE_T count) {
	UINT8* dstBuff = (UINT8*)destination;
	const UINT8* srcBuff = (UINT8*)source;
	for (SIZE_T i = 0; i < count; i++) dstBuff[i] = srcBuff[i];
}

VOID FillMemory(VOID* destination, UINT8 value, SIZE_T count) {
	UINT8* dstBuff = (UINT8*)destination;
	for (SIZE_T i = 0; i < count; i++) dstBuff[i] = value;
}

BOOL CompareMemory(const VOID* first, const VOID* second, SIZE_T count) {
	UINT8* first8 = (UINT8*)first;
	UINT8* second8 = (UINT8*)second;
	for (SIZE_T i = 0; i < count; i++) if (first8[i] != second8[i]) return FALSE;
	return TRUE;
}