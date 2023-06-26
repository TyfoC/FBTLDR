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

	//	FPU
	// InstallFPU();

	//	PIT
	InstallPIT(PIT_SOFTWARE_FREQUENCY);

	//	ACPI
	if (!InstallACPI()) PutString("Warning: failed to initialize ACPI driver, further work may lead to failures!\r\n", BIOS_COLOR_YELLOW);

	if (!InstallPS2Ctrl()) {
		PutString("Error: failed to initialize PS/2 Controller!\r\n", BIOS_COLOR_LIGHT_RED);
		ShutdownKernel(5);
	}
	else {
		PrintFormatted("First PS/2 Controller Port Available: %u\r\n", BIOS_COLOR_YELLOW, PS2CtrlFirstPortAvailable());
		PrintFormatted("Second PS/2 Controller Port Available: %u\r\n", BIOS_COLOR_YELLOW, PS2CtrlSecondPortAvailable());
		if (!PS2DevsInit()) {
			PutString("Error: failed to initialize PS/2 devices!\r\n", BIOS_COLOR_LIGHT_RED);
			ShutdownKernel(5);
		}

		BOOL ps2KbdInstalled = InstallPS2Kbd();
		if (!ps2KbdInstalled) PutString("Warning: failed to initialize PS/2 kbd driver!\r\n", BIOS_COLOR_LIGHT_RED);

		BOOL ps2MouseInstalled = InstallPS2Mouse();
		if (!ps2MouseInstalled && !ps2KbdInstalled) {
			PutString("Error: failed to initialize PS/2 mouse!\r\n", BIOS_COLOR_LIGHT_RED);
			ShutdownKernel(5);
		}
	}

	CHAR offChar;
	BIOS_COLOR offColor;
	SIZE_T off = NPOS, msX = TERMINAL_WIDTH / 2, msY = TERMINAL_HEIGHT / 2;
	SIZE_T prevX = msX, prevY = msY;
	UINT8* termBuff = GetTerminalBuffer();
	while (TRUE) {
		msX = PS2MouseGetX(PS2_MOUSE_WIDTH, 9);
		msY = PS2MouseGetY(PS2_MOUSE_HEIGHT, 16);

		if (msX != prevX || msY != prevY) {
			termBuff[off] = offChar;
			termBuff[off + 1] = offColor;

			SetCursorPosition((STRING_POSITION){ msY, msX });
			offChar = GetTerminalCharByOffset();
			offColor = GetTerminalCharColorByOffset();
			off = GetCursorOffset();

			termBuff[off] = 'O';
			termBuff[off + 1] = BIOS_COLOR_LIGHT_GREEN;
		}

		prevX = msX;
		prevY = msY;
	}

	ShutdownKernel(3);
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