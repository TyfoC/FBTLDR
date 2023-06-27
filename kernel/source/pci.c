#include <pci.h>

static PCI_ENUM_RESULT PCIDevices = { 0, 0 };

VOID InstallPCI(VOID) {
	PCICheckAllBuses(&PCIDevices);
}

SIZE_T PCIGetDevicesCount(VOID) {
	return PCIDevices.Count;
}

BOOL PCIGetDevice(SIZE_T index, PCI_DEVICE* pciDev) {
	if (index >= PCIDevices.Count) return FALSE;
	*pciDev = PCIDevices.Devices[index];
	return TRUE;
}

#include <terminal.h>
VOID PCICheckAllBuses(PCI_ENUM_RESULT* res) {
	PCI_ENUM_RESULT tmp = { 0, 0 };
	PCI_DEVICE* tmpDevs;

	UINT8 hdrType = PCIGetHeaderType(0, 0, 0);
	if (hdrType & PCI_HEADER_TYPE_MF) {
		for (UINT8 bus = 0; bus < 8; bus++) {
			if (PCIGetVendorID(0, 0, bus) != PCI_INVALID_VENDOR_ID) break;
			PCICheckBus(bus, &tmp);
			tmpDevs = ConcatMemory(res->Devices, tmp.Devices, sizeof(PCI_DEVICE), res->Count, tmp.Count);
			if (!tmpDevs) {
				FreePhysicalMemory(tmp.Devices);
				FreePhysicalMemory(res->Devices);
				res->Count = 0;
				return;
			}

			res->Devices = tmpDevs;
			res->Count += tmp.Count;

			FreePhysicalMemory(tmp.Devices);
			tmp = (PCI_ENUM_RESULT){ 0, 0 };
		}
	}
	else PCICheckBus(0, res);
}

VOID PCICheckBus(UINT8 bus, PCI_ENUM_RESULT* res) {
	PCI_ENUM_RESULT tmp = { 0, 0 };
	PCI_DEVICE* tmpDevs;

	for (UINT8 slot = 0; slot < 32; slot++) {
		PCICheckSlot(bus, slot, &tmp);
		tmpDevs = ConcatMemory(res->Devices, tmp.Devices, sizeof(PCI_DEVICE), res->Count, tmp.Count);
		if (!tmpDevs) {
			FreePhysicalMemory(tmp.Devices);
			FreePhysicalMemory(res->Devices);
			res->Count = 0;
			return;
		}

		res->Devices = tmpDevs;
		res->Count += tmp.Count;
		FreePhysicalMemory(tmp.Devices);
		tmp = (PCI_ENUM_RESULT){ 0, 0 };
	}
}

VOID PCICheckSlot(UINT8 bus, UINT8 slot, PCI_ENUM_RESULT* res) {
	PCI_ENUM_RESULT tmp = { 0, 0 };
	PCI_DEVICE* tmpDevs;

	UINT16 vendorID = PCIGetVendorID(bus, slot, 0);
	if (vendorID == PCI_INVALID_VENDOR_ID) return;

	PCICheckFunction(bus, slot, 0, res);
	UINT8 hdrType = PCIGetHeaderType(bus, slot, 0);
	if (hdrType & PCI_HEADER_TYPE_MF) {
		for (UINT8 function = 1; function < 8; function++) {
			if (PCIGetVendorID(bus, slot, function) != PCI_INVALID_VENDOR_ID) {
				PCICheckFunction(bus, slot, function, &tmp);
				tmpDevs = ConcatMemory(res->Devices, tmp.Devices, sizeof(PCI_DEVICE), res->Count, tmp.Count);
				if (!tmpDevs) {
					FreePhysicalMemory(tmp.Devices);
					FreePhysicalMemory(res->Devices);
					res->Count = 0;
					return;
				}

				res->Devices = tmpDevs;
				res->Count += tmp.Count;
				FreePhysicalMemory(tmp.Devices);
				tmp = (PCI_ENUM_RESULT){ 0, 0 };
			}
		}
	}
}

VOID PCICheckFunction(UINT8 bus, UINT8 slot, UINT8 function, PCI_ENUM_RESULT* res) {
	PCI_ENUM_RESULT tmp = { 0, 0 };
	PCI_DEVICE* tmpDevs;

	UINT16 vendorID = PCIGetVendorID(bus, slot, function);
	UINT16 deviceID = PCIGetDeviceID(bus, slot, function);
	UINT8 baseClass = PCIGetClass(bus, slot, function);
	UINT8 subClass = PCIGetSubclass(bus, slot, function);

	if ((baseClass == PCI_CLASS_BRIDGE) && (subClass == PCI_SUBCLASS_BRIDGE_PCI2PCI1 || subClass == PCI_SUBCLASS_BRIDGE_PCI2PCI2)) {
		PCICheckBus((UINT8)((PCIConfigReadWord(bus, slot, function, 0x0C) >> 8) & 0xFF), &tmp);
		tmpDevs = ConcatMemory(res->Devices, tmp.Devices, sizeof(PCI_DEVICE), res->Count, tmp.Count);
		if (!tmpDevs) {
			FreePhysicalMemory(tmp.Devices);
			FreePhysicalMemory(res->Devices);
			res->Count = 0;
			return;
		}

		res->Devices = tmpDevs;
		res->Count += tmp.Count;
		FreePhysicalMemory(tmp.Devices);
		tmp = (PCI_ENUM_RESULT){ 0, 0 };
	}
	else {
		tmpDevs = ReallocatePhysicalMemory(res->Devices, sizeof(PCI_DEVICE) * (res->Count + 1));
		if (tmpDevs) {
			res->Devices = tmpDevs;
			res->Devices[res->Count].VendorID = vendorID;
			res->Devices[res->Count].DeviceID = deviceID;
			res->Devices[res->Count].Class = baseClass;
			res->Devices[res->Count].SubClass = subClass;
			res->Devices[res->Count].Bus = bus;
			res->Devices[res->Count].Slot = slot;
			res->Devices[res->Count].Function = function;
			++res->Count;
		}
	}

	return;
}

UINT32 PCIConfigReadLong(UINT8 bus, UINT8 slot, UINT8 function, SIZE_T longIndex) {
	UINT32 address = (UINT32)(
		(UINT32)PCI_CONFIG_FLAG_ENABLE |
		((UINT32)bus << 16) |
		((UINT32)slot << 11) |
		((UINT32)function << 8) |
		((UINT32)longIndex << 2)
	);

	OutLong(PCI_PORT_CONFIG_ADDRESS, address);
	return InLong(PCI_PORT_CONFIG_DATA);
}

UINT16 PCIConfigReadWord(UINT8 bus, UINT8 slot, UINT8 function, SIZE_T wordIndex) {
	UINT32 longValue = PCIConfigReadLong(bus, slot, function, (wordIndex & 0x7E) >> 1);
	return (UINT16)((longValue >> ((wordIndex & 1) << 4)) & 0xFFFF);
}

UINT8 PCIConfigReadByte(UINT8 bus, UINT8 slot, UINT8 function, SIZE_T byteIndex) {
	UINT16 wordValue = PCIConfigReadWord(bus, slot, function, (byteIndex & 0xFE) >> 1);
	return (UINT8)((wordValue >> ((byteIndex & 1) << 3)) & 0xFF);
}

UINT16 PCIGetVendorID(UINT8 bus, UINT8 slot, UINT8 function) {
	return PCIConfigReadWord(bus, slot, function, PCI_WORD_INDEX_VENDOR_ID);
}

UINT16 PCIGetDeviceID(UINT8 bus, UINT8 slot, UINT8 function) {
	return PCIConfigReadWord(bus, slot, function, PCI_WORD_INDEX_DEVICE_ID);
}

UINT8 PCIGetClass(UINT8 bus, UINT8 slot, UINT8 function) {
	return (UINT8)((PCIConfigReadWord(bus, slot, function, PCI_WORD_INDEX_SUB_CLS_CLS) >> 8) & 0xFF);
}

UINT8 PCIGetSubclass(UINT8 bus, UINT8 slot, UINT8 function) {
	return (UINT8)(PCIConfigReadWord(bus, slot, function, PCI_WORD_INDEX_SUB_CLS_CLS) & 0xFF);
}

UINT8 PCIGetHeaderType(UINT8 bus, UINT8 slot, UINT8 function) {
	return (UINT8)(PCIConfigReadWord(bus, slot, function, PCI_WORD_INDEX_HDR_TYPE_BIST) & 0xFF);
}

UINT8 PCIGetSecondaryBus(UINT8 bus, UINT8 slot, UINT8 function) {
	return (UINT8)((PCIConfigReadWord(bus, slot, function, PCI_WORD_INDEX_SEC_BUS_PRIM_BUS) >> 8) & 0xFF);
}