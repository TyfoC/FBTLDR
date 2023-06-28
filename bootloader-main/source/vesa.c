#include "vesa.h"

bool_t GetVesaInfoBlock(VesaInfoBlock* infoBlock) {
	uint16_t result;
	infoBlock->Signature = VESA_SIGNATURE;
	ASM("int $0x10":"=a"(result):"a"(0x4f00), "D"((size_t)infoBlock));
	return result == 0x004f;
}

bool_t GetVesaModeInfo(uint16_t modeNumber, VesaModeInfo* modeInfo) {
	uint16_t result;
	ASM("int $0x10":"=a"(result):"a"(0x4f01), "c"(modeNumber), "D"((size_t)modeInfo));
	return result == 0x004f;
}

bool_t SetVesaMode(uint16_t modeNumber) {
	uint16_t result;
	ASM("int $0x10":"=a"(result):"a"(0x4f02), "b"(modeNumber));
	return result == 0x004f;
}