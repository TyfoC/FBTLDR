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

	InitPMM(memMap, memMapLen);
	memMapLen = GetMemoryMapLength();

	//	GDT, KernelCodeSegValue, KernelDataSegValue
	InitGDTRegister(KernelGDT, KERNEL_GDT_ENTRIES_COUNT, &KernelGDTRegister);
	LoadGDTRegister(&KernelGDTRegister);
	KernelCodeSegValue = GetCodeSegValue();
	KernelDataSegValue = GetDataSegValue();

	//	IDT, ISRs, IRQs
	InitIDTRegister(KernelIDT, KERNEL_IDT_ENTRIES_COUNT, &KernelIDTRegister);
	LoadIDTRegister(&KernelIDTRegister);
	InitSoftwareIntHandlers(KernelIDT, KernelCodeSegValue);
	RemapPIC(0x20, 0x28);
	MaskPIC();
	InitHardwareIntHandlers(KernelIDT, KernelCodeSegValue);
	ENABLE_HARDWARE_INTERRUPTS();

	//	PIT
	InitPIT(PIT_SOFTWARE_FREQUENCY);

	//	ACPI
	UINT8 acpiVerMajor, acpiVerMinor;
	UINT16 bootArchFlags;
	if (!InitACPI()) PutString("Warning: failed to initialize ACPI driver, further work may lead to failures!\r\n", BIOS_COLOR_YELLOW);
	else {
		acpiVerMajor = GetACPIMajorVersion();
		acpiVerMinor = GetACPIMinorVersion();
		bootArchFlags = GetBootArchitectureFlags();
		PrintFormatted("ACPI version: %u.%u\r\n", BIOS_COLOR_WHITE, acpiVerMajor, acpiVerMinor & 0x03);
		
		if (acpiVerMajor >= 2 && !(bootArchFlags & BOOT_ARCH_FLAG_8042)) {
			PutString("Error: PS/2 keyboard not found! User input not possible!\r\n", BIOS_COLOR_LIGHT_RED);
			ShutdownKernel(5);
		}
	}

	ShutdownKernel(5);
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