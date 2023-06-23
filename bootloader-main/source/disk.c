#include <disk.h>

void LbaToChs(uint32_t value, uint8_t* sectorNumber, uint8_t* headIndex, uint16_t* cylinderIndex) {
	uint32_t tmp = value / MAX_SECTORS_COUNT_PER_OPERATION;
	*sectorNumber = (value % MAX_SECTORS_COUNT_PER_OPERATION) + 1;
	*headIndex = tmp % 16;
	*cylinderIndex = tmp / 16;
}

uint8_t ReadSectors(
	uint8_t diskIndex,
	uint32_t lbaValue,
	uint32_t count,
	size_t buffSeg,
	size_t buffOff
) {
	uint32_t longParts = count / MAX_SECTORS_COUNT_PER_OPERATION;
	uint32_t remainder = count % MAX_SECTORS_COUNT_PER_OPERATION;

	uint8_t sectorNumber, headIndex;
	uint16_t cylinderIndex;

	if (longParts) {
		for (uint32_t i = 0; i < longParts; i++) {
			LbaToChs(lbaValue, &sectorNumber, &headIndex, &cylinderIndex);
			if (!ReadSectorsLow(diskIndex, sectorNumber, headIndex, cylinderIndex, MAX_SECTORS_COUNT_PER_OPERATION, buffSeg, buffOff)) {
				return FALSE;
			}

			AddValueToSegOff(&buffSeg, &buffOff, MAX_SECTORS_LENGTH_PER_OPERATION);
			lbaValue += MAX_SECTORS_COUNT_PER_OPERATION;
		}
	}

	LbaToChs(lbaValue, &sectorNumber, &headIndex, &cylinderIndex);
	return remainder ? ReadSectorsLow(
		diskIndex,
		sectorNumber,
		headIndex,
		cylinderIndex,
		remainder,
		buffSeg,
		buffOff
	) : TRUE;
}