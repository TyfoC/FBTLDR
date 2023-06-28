#include <bootloader.h>
#include <kernel.h>
#include <a20-line.h>
#include <disk.h>
#include <terminal.h>
#include <meminfo.h>
#include <cpu.h>
#include <vesa.h>
#include <user-config.h>

VesaInfoBlock InfoBlock;
VesaModeInfo ModeInfo;
UserConfig UserCfg;

extern void BootloaderMain(uint8_t diskIndex, uint16_t kernelSectorLba) {
	if (!ResetDiskSystem(diskIndex)) PutString("[WARNING] Failed to reset disk system!\r\n");
	if (!ReadSectors(diskIndex, kernelSectorLba, 1, 0, BOOTLOADER_END_ADDRESS)) {
		PutString("[ERROR] Failed to load kernel header into memory!\r\n");
		STOP_EXECUTION();
	}

	KernelHeader kernelHdr = *(KernelHeader*)BOOTLOADER_END_ADDRESS;
	uint32_t alignedKernelEndAddress = ALIGN_UP(kernelHdr.BaseAddress + kernelHdr.FileLength, 512);
	uint32_t kernelLengthInSectors = ALIGN_UP(kernelHdr.FileLength, 512) / 512;
	if (!ReadSectors(diskIndex, kernelSectorLba, kernelLengthInSectors, KERNEL_BASE_SEG_VALUE, KERNEL_BASE_OFF_VALUE)) {
		PutString("[ERROR] Failed to load kernel into memory!\r\n");
		STOP_EXECUTION();
	}

	size_t userCfgSectorLba = kernelSectorLba + kernelLengthInSectors;
	if (!ReadSectors(diskIndex, userCfgSectorLba, 1, 0, (size_t)&UserCfg)) {
		PutString("[ERROR] Failed to read user config!\r\n");
		STOP_EXECUTION();
	}

	KernelGdt[0] = CreateGdtEntry(0, 0, 0, 0);
	KernelGdt[KERNEL_GDT_CODE_SEG_INDEX] = CreateGdtEntry(
		0, 0xffffff,
		GDT_ACCESS_READ_WRITE | GDT_ACCESS_EXECUTABLE | GDT_ACCESS_CODE_OR_DATA | GDT_ACCESS_PRESENT,
		GDT_FLAG_PROTECTED_MODE | GDT_FLAG_4KB_PAGES
	);

	KernelGdt[KERNEL_GDT_DATA_SEG_INDEX] = CreateGdtEntry(
		0, 0xffffff,
		GDT_ACCESS_READ_WRITE | GDT_ACCESS_CODE_OR_DATA | GDT_ACCESS_PRESENT,
		GDT_FLAG_PROTECTED_MODE | GDT_FLAG_4KB_PAGES
	);

	KernelGdtRegister = CreateGdtRegister(KernelGdt, KERNEL_GDT_ENTRIES_COUNT);

	size_t memoryMapSeg = (alignedKernelEndAddress >> 4) & 0xf000, memoryMapOff = alignedKernelEndAddress & 0xffff;
	KernelInitializationData* kernelInitData = (KernelInitializationData*)BOOTLOADER_BASE_ADDRESS;
	kernelInitData->MemoryMapAddress = alignedKernelEndAddress;
	if (!GetMemoryMap(memoryMapSeg, memoryMapOff, (uint32_t*)((size_t)kernelInitData + sizeof(uint32_t)))) {
		PutString("[ERROR] Failed to get memory map!\r\n");
		STOP_EXECUTION();
	}

	kernelInitData->GdtAddress = (uint32_t)&KernelGdt[0];
	kernelInitData->GdtEntriesCount = KERNEL_GDT_ENTRIES_COUNT;
	kernelInitData->GdtCodeSegIndex = KERNEL_GDT_CODE_SEG_INDEX;
	kernelInitData->GdtDataSegIndex = KERNEL_GDT_DATA_SEG_INDEX;
	kernelInitData->PciSupportBits = (uint32_t)GetPciSupport();
	kernelInitData->UserKernelConfig = UserCfg;

	SwitchToProtectedMode(
		&KernelGdtRegister, kernelHdr.EntryPoint,
		KERNEL_GDT_CODE_SEG_OFF, KERNEL_GDT_DATA_SEG_OFF,
		kernelHdr.StackTopAddress,
		(uint32_t)kernelInitData
	);
}