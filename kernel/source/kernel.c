#include <kernel.h>

extern VOID KernelMain(KERNEL_INIT_DATA* initData) {
	SetCursorPosition((STRING_POSITION){ 16, 0 });
	PrintFormatted(
		"[%aINFO%r] Kernel loaded, preparing environment...\r\n", BIOS_COLOR_WHITE,
		BIOS_COLOR_LIGHT_GREEN,
		initData->MemoryMapLength, initData->MemoryMapAddress,
		initData->GdtEntriesCount, initData->GdtAddress
	);

	while (1);
}