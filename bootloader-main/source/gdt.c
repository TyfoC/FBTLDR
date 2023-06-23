#include <gdt.h>

GdtRegister CreateGdtRegister(const GdtEntry* gdtEntries, size_t count) {
	GdtRegister gdtReg = {
		sizeof(GdtEntry) * count - 1,
		(uint32_t)gdtEntries
	};

	return gdtReg;
}

GdtEntry CreateGdtEntry(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
	GdtEntry gdtEntry = {
		(uint16_t)(limit & 0xffff),
		(uint16_t)(base & 0xffff),
		(uint8_t)((base >> 16) & 0xff),
		access,
		(uint8_t)(((limit >> 16) & 0x0f) | ((flags & 0x0f) << 4)),
		(uint8_t)((base >> 24) & 0xff)
	};

	return gdtEntry;
}