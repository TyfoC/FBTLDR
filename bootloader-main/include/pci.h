#pragma once
#ifndef PCI
#define PCI

#include "typedefs.h"

#define PCI_SUPPORT_FIRST_ACCESS_MECHANISM			0x01
#define PCI_SUPPORT_SECOND_ACCESS_MECHANISM			0x02
#define PCI_SUPPORT_CHECK_COMPLETED_SUCCESSFULLY	0x04
#define PCI_SUPPORT_FIRST_SPEC_CYCLE_GEN_MECHANISM	0x10
#define PCI_SUPPORT_SECOND_SPEC_CYCLE_GEN_MECHANISM	0x20

typedef uint8_t pci_support_t;

pci_support_t GetPciSupport(void);

#endif