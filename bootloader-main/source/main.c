#include <bootloader.h>
#include <kernel.h>
#include <a20-line.h>
#include <disk.h>
#include <terminal.h>
#include <cpu.h>

extern void BootloaderMain(uint8_t diskIndex, uint16_t kernelSectorLba) {
	PutString("[INFO] The main part of the bootloader is executed\r\n");
	PrintFormatted(
		"\tBase address: 0x%x, end address: 0x%x, length: 0x%x bytes\r\n",
		BOOTLOADER_BASE_ADDRESS, BOOTLOADER_END_ADDRESS, BOOTLOADER_SIZE
	);

	PrintFormatted(
		"\tStart sector: 0x%x, end sector: 0x%x, length: 0x%x sector(s)\r\n",
		BOOTLOADER_START_SECTOR, BOOTLOADER_END_SECTOR, BOOTLOADER_LENGTH
	);

	PutString("[INFO] Initializing GDT...\r\n");
	KernelGdt[0] = CreateGdtEntry(0, 0, 0, 0);	//	NULL
	KernelGdt[KERNEL_GDT_CODE_SEG_INDEX] = CreateGdtEntry(				//	CODESEG
		0, 0xffffff,
		GDT_ACCESS_READ_WRITE | GDT_ACCESS_EXECUTABLE | GDT_ACCESS_CODE_OR_DATA | GDT_ACCESS_PRESENT,
		GDT_FLAG_PROTECTED_MODE | GDT_FLAG_4KB_PAGES
	);
	KernelGdt[KERNEL_GDT_DATA_SEG_INDEX] = CreateGdtEntry(				//	DATASEG
		0, 0xffffff,
		GDT_ACCESS_READ_WRITE | GDT_ACCESS_CODE_OR_DATA | GDT_ACCESS_PRESENT,
		GDT_FLAG_PROTECTED_MODE | GDT_FLAG_4KB_PAGES
	);

	KernelGdtRegister = CreateGdtRegister(KernelGdt, KERNEL_GDT_ENTRIES_COUNT);
	PrintFormatted("\tGDT(R: 0x%x) Addr: 0x%x, entries cnt: 0x%x\r\n", (size_t)&KernelGdtRegister, (size_t)KernelGdt, KERNEL_GDT_ENTRIES_COUNT);
	for (size_t i = 0; i < KERNEL_GDT_ENTRIES_COUNT; i++) {
		PrintFormatted(
			"\tGDT Entry #%u: BaseAddr: 0x%x, Lim: 0x%x, AccByte: 0x%x, Flags: 0x%x\r\n",
			i + 1,
			KernelGdt[i].BaseLow | (KernelGdt[i].BaseMiddle << 16) | (KernelGdt[i].BaseHigh << 24),
			KernelGdt[i].LimitLow | ((KernelGdt[i].LimitHighAndFlags & 0x0f) << 16),
			KernelGdt[i].AccessByte, (KernelGdt[i].LimitHighAndFlags >> 4) & 0x0f
		);
	}
	
	PrintFormatted("[INFO] Initializing disk system (for disk with index 0x%x)...\r\n", diskIndex);
	if (!ResetDiskSystem(diskIndex)) PutString("[WARNING] Failed to reset disk system!\r\n");

	PutString("[INFO] Loading kernel header into memory...\r\n");
	if (!ReadSectors(diskIndex, kernelSectorLba, 1, 0, BOOTLOADER_END_ADDRESS)) {
		PutString("[ERROR] Failed to load kernel header into memory!\r\n");
		STOP_EXECUTION();
	}

	KernelHeader kernelHdr = *(KernelHeader*)BOOTLOADER_END_ADDRESS;
	uint32_t kernelLengthInSectors = ALIGN_UP(kernelHdr.FileLength, 512) / 512;
	PrintFormatted(
		"\tBase address: 0x%x, length: 0x%x bytes,\r\n\tEntry point address: 0x%x, stack top address: 0x%x\r\n",
		kernelHdr.BaseAddress, kernelHdr.FileLength, kernelHdr.EntryPoint, kernelHdr.StackTopAddress
	);

	PutString("[INFO] Loading kernel into memory...\r\n");
	if (!ReadSectors(diskIndex, kernelSectorLba, kernelLengthInSectors, KERNEL_BASE_SEG_VALUE, KERNEL_BASE_OFF_VALUE)) {
		PutString("[ERROR] Failed to load kernel into memory!\r\n");
		STOP_EXECUTION();
	}

	PutString("[INFO] Jumping into kernel...\r\n");
	SwitchToProtectedMode(
		&KernelGdtRegister, kernelHdr.EntryPoint,
		KERNEL_GDT_CODE_SEG_OFF, KERNEL_GDT_DATA_SEG_OFF,
		kernelHdr.StackTopAddress
	);
}