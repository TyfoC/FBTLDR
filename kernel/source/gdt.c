#include <gdt.h>

VOID InitGDTRegister(const GDT_ENTRY* gdt, SIZE_T entriesCount, GDT_REGISTER* gdtReg) {
	gdtReg->Size = sizeof(GDT_ENTRY) * entriesCount - 1;
	gdtReg->Offset = (UINT32)gdt;
}

VOID InitGDTEntry(UINT32 base, UINT32 limit, UINT8 access, UINT8 flags, GDT_ENTRY* gdtEntry) {
	gdtEntry->LimitLow = limit & 0xffff;
	gdtEntry->BaseLow = base & 0xffffff;
	gdtEntry->Access = access & 0x0f;
	gdtEntry->LimitHigh = (limit >> 16) & 0xff;
	gdtEntry->Flags = flags & 0x0f;
	gdtEntry->BaseHigh = (base >> 24) & 0xff;
}