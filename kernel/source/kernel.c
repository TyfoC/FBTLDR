#include <kernel.h>

extern VOID KernelMain(KERNEL_INIT_DATA* initData) {
	CopyMemory(KernelGDT, initData->GDTPointer, sizeof(GDT_ENTRY) * KERNEL_GDT_ENTRIES_COUNT);
	SetCursorPosition((STRING_POSITION){ 16, 0 });

	//	PMM
	MEMORY_REGION_DESCRIPTOR* memMap = (MEMORY_REGION_DESCRIPTOR*)initData->MemoryMapAddress;
	MEMORY_REGION_DESCRIPTOR extraMemRegs[] = {
		{ KERNEL_BASE_ADDRESS, KERNEL_SIZE, MEMORY_REGION_TYPE_RESERVED, 0 },	//	kernel
		{ 0, 0x500, MEMORY_REGION_TYPE_RESERVED, 0 },							//	BDA
		{ 0x80000, 0x80000, MEMORY_REGION_TYPE_RESERVED, 0 },					//	EBDA & other data
		{ 0xF00000, 0x100000, MEMORY_REGION_TYPE_RESERVED, 0 }					//	ISA hole
	};

	SIZE_T memMapLen = TransformBIOSMemoryMap(
		(ADDRESS_RANGE_DESCRIPTOR*)initData->MemoryMapAddress,
		(MEMORY_REGION_DESCRIPTOR*)initData->MemoryMapAddress,
		initData->MemoryMapLength
	);

	memMapLen = RemoveMemoryRegions(
		memMap, extraMemRegs, memMapLen,
		sizeof(extraMemRegs) / sizeof(extraMemRegs[0])
	);

	InstallPMM(memMap, memMapLen);
	memMapLen = GetMemoryMapLength();
	SIZE_T usableMemSize = 0, reservedMemSize;
	MEMORY_REGION_DESCRIPTOR mrd;
	for (SIZE_T i = 0; i < memMapLen; i++) {
		GetMemoryRegion(i, &mrd);
		if (mrd.Type == MEMORY_REGION_TYPE_USABLE || mrd.Type == MEMORY_REGION_TYPE_RECLAIMABLE) usableMemSize += mrd.Size;
		else reservedMemSize += mrd.Size;
	}

	PrintFormatted("Memory regions detected: %u\r\n", BIOS_COLOR_WHITE, memMapLen);
	PrintFormatted("Size of usable memory: 0x%xu bytes\r\n", BIOS_COLOR_WHITE, usableMemSize);
	PrintFormatted("Size of reserved memory: 0x%xu bytes\r\n", BIOS_COLOR_WHITE, reservedMemSize);

	//	GDT, KernelCodeSegValue, KernelDataSegValue
	InitGDTR(KernelGDT, KERNEL_GDT_ENTRIES_COUNT, &KernelGDTRegister);
	LoadGDTRegister(&KernelGDTRegister);
	KernelCodeSegValue = GetCodeSegValue();
	KernelDataSegValue = GetDataSegValue();

	//	IDT, ISRs, IRQs
	InitIDTR(KernelIDT, KERNEL_IDT_ENTRIES_COUNT, &KernelIDTRegister);
	LoadIDTRegister(&KernelIDTRegister);
	InstallSoftwareIntHandlers(KernelIDT, KernelCodeSegValue);
	RemapPIC(0x20, 0x28);
	MaskPIC();
	InstallHardwareIntHandlers(KernelIDT, KernelCodeSegValue);
	ENABLE_HARDWARE_INTERRUPTS();

	//	PIT
	InstallPIT(PIT_SOFTWARE_FREQUENCY);

	//	PCI
	InstallPCI();
	PrintFormatted("Number of PCI devices: %u\r\n", BIOS_COLOR_WHITE, PCIGetDevicesCount());

	//	ACPI
	if (!InstallACPI()) PutString("Warning: failed to initialize ACPI driver, further work may lead to failures!\r\n", BIOS_COLOR_YELLOW);
	else PrintFormatted("ACPI version: %u.%u\r\n", BIOS_COLOR_WHITE, ACPIGetMajorVersion(), ACPIGetMinorVersion() & 0x0F);

	if (!InstallPS2Ctrl()) PutString("PS/2 controller not found or failed to initialize\r\n", BIOS_COLOR_YELLOW);
	else {
		if (!PS2DevsInit()) PutString("PS/2 devices not found or failed to initialize\r\n", BIOS_COLOR_YELLOW);
		else {
			if (!InstallPS2Kbd()) PutString("PS/2 keyboard not found or failed to initialize\r\n", BIOS_COLOR_YELLOW);
			if (!InstallPS2Mouse()) PutString("PS/2 mouse not found or failed to initialize\r\n", BIOS_COLOR_YELLOW);
		}
	}

	//	IDE
	if (!InstallIDE()) PrintFormatted("PCI IDE controllers not found or failed to initialize\r\n", BIOS_COLOR_YELLOW);
	else PrintFormatted("Number of PCI IDE controllers: %u\r\n", BIOS_COLOR_WHITE, IDEGetCtrlsCount());

	ShutdownKernel(15);
	STOP();
}

VOID ShutdownKernel(SIZE_T secondsCount) {
	PutString("Shutdown after ", BIOS_COLOR_YELLOW);
	SIZE_T cursorOffset = GetCursorOffset();
	for (SIZE_T i = 0; i < secondsCount; i++) {
		PrintFormatted("%u %aseconds...", BIOS_COLOR_LIGHT_BLUE, secondsCount - i, BIOS_COLOR_YELLOW);
		Sleep(1000);
		SetCursorOffset(cursorOffset);
	}

	if (!ACPIPowerOff()) {
		PutString("Shutdown failed!\r\n", BIOS_COLOR_LIGHT_RED);
		HLT();
	}
	else PutString("Shutdown successfull!\r\n", BIOS_COLOR_LIGHT_GREEN);
}

GDT_REGISTER KernelGDTRegister;
GDT_ENTRY KernelGDT[KERNEL_GDT_ENTRIES_COUNT] = {};
IDT_REGISTER KernelIDTRegister;
IDT_ENTRY KernelIDT[KERNEL_IDT_ENTRIES_COUNT] = {};
UINT16 KernelCodeSegValue;
UINT16 KernelDataSegValue;