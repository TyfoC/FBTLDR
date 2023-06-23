#include <kernel.h>

extern VOID KernelMain(KERNEL_INIT_DATA* initData) {
	SetCursorPosition((STRING_POSITION){ 16, 0 });
	CopyMemory(KernelGDT, initData->GDTPointer, sizeof(GDT_ENTRY) * KERNEL_GDT_ENTRIES_COUNT);
	InitGDTRegister(KernelGDT, KERNEL_GDT_ENTRIES_COUNT, &KernelGDTRegister);
	LoadGDTRegister(&KernelGDTRegister);

	PrintFormatted(
		"[%aINFO%r] Kernel loaded, received information from the bootloader:\r\n",
		BIOS_COLOR_WHITE, BIOS_COLOR_LIGHT_GREEN
	);

	PrintFormatted("\tGDT:\tAddress: 0x%xu, Entries count: %u\r\n", BIOS_COLOR_WHITE, &KernelGDT[0], KERNEL_GDT_ENTRIES_COUNT);
	for (SIZE_T i = 0; i < KERNEL_GDT_ENTRIES_COUNT; i++) {
		PrintFormatted(
			"\t\t%u) Base: 0x%xu, Limit: 0x%xu, Access: 0x%xu, Flags: 0x%xu\r\n", BIOS_COLOR_WHITE,
			i + 1,
			KernelGDT[i].BaseLow | (KernelGDT[i].BaseHigh << 24),
			KernelGDT[i].LimitLow | (KernelGDT[i].LimitHigh << 16),
			KernelGDT[i].Access,
			KernelGDT[i].Flags
		);
	}

	while (1);
}

GDT_REGISTER KernelGDTRegister;
GDT_ENTRY KernelGDT[KERNEL_GDT_ENTRIES_COUNT];