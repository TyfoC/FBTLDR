#include <cpu-desc-tables.h>

VOID InitGDTRegister(const GDT_ENTRY* gdt, SIZE_T entriesCount, GDT_REGISTER* gdtReg) {
	gdtReg->Size = sizeof(GDT_ENTRY) * entriesCount - 1;
	gdtReg->Offset = (UINT32)gdt;
}

VOID InitGDTEntry(UINT32 base, UINT32 limit, UINT8 access, UINT8 flags, GDT_ENTRY* gdtEntry) {
	gdtEntry->LimitLow = limit & 0xFFFF;
	gdtEntry->BaseLow = base & 0xFFFFFF;
	gdtEntry->Access = access & 0x0F;
	gdtEntry->LimitHigh = (limit >> 16) & 0xFF;
	gdtEntry->Flags = flags & 0x0F;
	gdtEntry->BaseHigh = (base >> 24) & 0xFF;
}

VOID InitIDTRegister(const IDT_ENTRY* idt, SIZE_T entriesCount, IDT_REGISTER* idtReg) {
	idtReg->Size = sizeof(IDT_ENTRY) * entriesCount - 1;
	idtReg->Offset = (UINT32)idt;
}

VOID InitIDTEntry(UINT32 base, SEGMENT_SELECTOR codeSegSelector, UINT8 gateType, UINT8 flags, IDT_ENTRY* idtEntry) {
	idtEntry->BaseLow = base & 0xFFFF;
	idtEntry->CodeSegSelector = codeSegSelector;
	idtEntry->Reserved = 0;
	idtEntry->GateType = gateType & 0x0F;
	idtEntry->Flags = flags & 0x0E;
	idtEntry->BaseHigh = (base >> 16) & 0xFFFF;
}