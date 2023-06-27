#pragma once
#ifndef PCI
#define PCI

#include "port.h"
#include "memory.h"

#define PCI_PORT_CONFIG_ADDRESS			0xCF8
#define PCI_PORT_CONFIG_DATA			0xCFC

#define PCI_CONFIG_FLAG_ENABLE			0x80000000

#define PCI_WORD_INDEX_VENDOR_ID		0x00
#define PCI_WORD_INDEX_DEVICE_ID		0x01
#define PCI_WORD_INDEX_COMMAND			0x02
#define PCI_WORD_INDEX_STATUS			0x03
#define PCI_WORD_INDEX_REV_ID_PROG_IF	0x04
#define PCI_WORD_INDEX_SUB_CLS_CLS		0x05
#define PCI_WORD_INDEX_CLSZ_LATENCY_TMR	0x06
#define PCI_WORD_INDEX_HDR_TYPE_BIST	0x07
#define PCI_WORD_INDEX_SEC_BUS_PRIM_BUS	0x0C

#define PCI_HEADER_TYPE_MF				0x80
#define PCI_INVALID_VENDOR_ID			0xFFFF

#define PCI_CLASS_MASS_STG_CTRL			0x01
#define PCI_CLASS_BRIDGE				0x06

#define PCI_SUBCLASS_MSTGCTRL_IDE_CTRL	0x01
#define PCI_SUBCLASS_BRIDGE_PCI2PCI1	0x04
#define PCI_SUBCLASS_BRIDGE_PCI2PCI2	0x09


typedef struct ATTRIB(__packed__) {
	UINT16	VendorID;
	UINT16	DeviceID;
	UINT8	Class;
	UINT8	SubClass;
	UINT8	Bus;
	UINT8	Slot;
	UINT8	Function;
} PCI_DEVICE;

typedef struct ATTRIB(__packed__) {
	PCI_DEVICE*	Devices;
	SIZE_T		Count;
} PCI_ENUM_RESULT;						// enumeration result

VOID InstallPCI(VOID);
SIZE_T PCIGetDevicesCount(VOID);
BOOL PCIGetDevice(SIZE_T index, PCI_DEVICE* pciDev);

VOID PCICheckAllBuses(PCI_ENUM_RESULT* res);
VOID PCICheckBus(UINT8 bus, PCI_ENUM_RESULT* res);
VOID PCICheckSlot(UINT8 bus, UINT8 slot, PCI_ENUM_RESULT* res);
VOID PCICheckFunction(UINT8 bus, UINT8 slot, UINT8 function, PCI_ENUM_RESULT* res);

UINT32 PCIConfigReadLong(UINT8 bus, UINT8 slot, UINT8 function, SIZE_T longIndex);
UINT16 PCIConfigReadWord(UINT8 bus, UINT8 slot, UINT8 function, SIZE_T wordIndex);
UINT8 PCIConfigReadByte(UINT8 bus, UINT8 slot, UINT8 function, SIZE_T byteIndex);
UINT16 PCIGetVendorID(UINT8 bus, UINT8 slot, UINT8 function);
UINT16 PCIGetDeviceID(UINT8 bus, UINT8 slot, UINT8 function);
UINT8 PCIGetClass(UINT8 bus, UINT8 slot, UINT8 function);
UINT8 PCIGetSubclass(UINT8 bus, UINT8 slot, UINT8 function);
UINT8 PCIGetHeaderType(UINT8 bus, UINT8 slot, UINT8 function);
UINT8 PCIGetSecondaryBus(UINT8 bus, UINT8 slot, UINT8 function);

#endif