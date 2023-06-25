#pragma once
#ifndef PCI
#define PCI

#include "port.h"
#include "memory.h"

#define PCI_CONFIG_ADDRESS			0x0CF8
#define PCI_CONFIG_DATA				0x0CFC
#define PCI_OFFSET_VENDOR_ID		0x00
#define PCI_OFFSET_DEVICE_ID		0x02
#define PCI_OFFSET_COMMAND			0x04
#define PCI_OFFSET_STATUS			0x06
#define PCI_OFFSET_REVISION_ID		0x08
#define PCI_OFFSET_PROG_INTERFACE	0x09
#define PCI_OFFSET_SUBCLASS			0x0A
#define PCI_OFFSET_CLASS			0x0B
#define PCI_OFFSET_CACHE_LINE_SIZE	0x0C
#define PCI_OFFSET_LATENCY_TIMER	0x0D
#define PCI_OFFSET_HEADER_TYPE		0x0E
#define PCI_OFFSET_BIST				0x0F

UINT16 PCIConfigReadWord(UINT8 bus, UINT8 slot, UINT8 function, UINT8 offset);

#endif