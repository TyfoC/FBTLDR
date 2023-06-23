#pragma once
#ifndef DISK
#define DISK

#include "segoff.h"

#define MAX_SECTORS_COUNT_PER_OPERATION		0x3f
#define MAX_SECTORS_LENGTH_PER_OPERATION	0x7e00

void LbaToChs(uint32_t value, uint8_t* sectorNumber, uint8_t* headIndex, uint16_t* cylinderIndex);

uint8_t ResetDiskSystem(uint8_t diskIndex);
uint8_t ReadSectorsLow(
	uint8_t diskIndex,
	uint8_t sectorNumber,
	uint8_t headIndex,
	uint16_t cylinderIndex,
	uint8_t count,
	size_t buffSeg,
	size_t buffOff
);

uint8_t ReadSectors(
	uint8_t diskIndex,
	uint32_t lbaValue,
	uint32_t count,
	size_t buffSeg,
	size_t buffOff
);

#endif