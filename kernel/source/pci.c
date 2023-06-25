#include <pci.h>

UINT16 PCIConfigReadWord(UINT8 bus, UINT8 slot, UINT8 function, UINT8 offset) {
	UINT32 alignedOffset = (UINT32)((bus << 16) | (slot << 11) | (function << 8) | (offset & 0xFC)) | 0x80000000;
	OutLong(PCI_CONFIG_ADDRESS, alignedOffset);
	return ((InLong(PCI_CONFIG_DATA) >> ((offset & 2) << 3)) & 0xFFFF);
}