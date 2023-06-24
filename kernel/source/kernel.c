#include <kernel.h>

extern VOID KernelMain(KERNEL_INIT_DATA* initData) {
	SetCursorPosition((STRING_POSITION){ 16, 0 });
	CopyMemory(KernelGDT, initData->GDTPointer, sizeof(GDT_ENTRY) * KERNEL_GDT_ENTRIES_COUNT);
	InitGDTRegister(KernelGDT, KERNEL_GDT_ENTRIES_COUNT, &KernelGDTRegister);
	LoadGDTRegister(&KernelGDTRegister);
	KernelCodeSegValue = GetCodeSegValue();
	KernelDataSegValue = GetDataSegValue();
	InitIDTRegister(KernelIDT, KERNEL_IDT_ENTRIES_COUNT, &KernelIDTRegister);
	LoadIDTRegister(&KernelIDTRegister);
	InitSoftwareIntHandlers(KernelIDT, KernelCodeSegValue);

	ASM("int $0x00");

	PutString("End...", BIOS_COLOR_YELLOW);
	STOP_EXECUTION();
}

GDT_REGISTER KernelGDTRegister;
GDT_ENTRY KernelGDT[KERNEL_GDT_ENTRIES_COUNT] = {};
IDT_REGISTER KernelIDTRegister;
IDT_ENTRY KernelIDT[KERNEL_IDT_ENTRIES_COUNT] = {};
UINT16 KernelCodeSegValue;
UINT16 KernelDataSegValue;