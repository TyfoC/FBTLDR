#pragma once
#ifndef ACPI
#define ACPI

#include "pit.h"

#define RSDP_SIGNATURE_SIZE					8
#define RSDP_SIGNATURE						"RSD PTR "
#define RSDT_SIGNATURE						"RSDT"
#define XSDT_SIGNATURE						"XSDT"
#define FADT_SIGNATURE						"FACP"
#define DSDT_SIGNATURE						"DSDT"
#define DSDT_S5_SIGNATURE					"_S5_"

#define BOOT_ARCH_FLAG_LEGACY_DEVICES		0x01
#define BOOT_ARCH_FLAG_8042					0x02	//	PS/2 controller exist
#define BOOT_ARCH_FLAG_VGA_NOT_PRESENT		0x04
#define BOOT_ARCH_FLAG_MSI_NOT_SUPPORTED	0x08
#define BOOT_ARCH_FLAG_PCIE_ASPM_CONTROLS	0x10	//	indicates to OSPM that it must not enable MSI
#define BOOT_ARCH_FLAG_CMOS_RTC_NOT_PRESENT	0x20

typedef struct ATTRIB_PACKED {
	CHAR						Signature[8];
	UINT8						Checksum;
	CHAR						OEMID[6];
	UINT8						Revision;
	UINT32						RSDTAddress;
} RSDP1;																	//	ACPI 1.X

typedef struct ATTRIB_PACKED {
	CHAR						Signature[8];
	UINT8						Checksum;
	CHAR						OEMID[6];
	UINT8						Revision;
	UINT32						RSDTAddress;
	UINT32						Length;
	UINT64						XSDTAddress;
	UINT8						ExtendedChecksum;
	UINT8						Reserved[3];
} RSDP2;																	//	ACPI 2.0+

typedef struct ATTRIB_PACKED {
	UINT32						Signature;
	UINT32						Length;
	UINT8						Revision;
	UINT8						Checksum;
	CHAR						OEMID[6];
	CHAR						OEMTableID[8];
	UINT32						OEMRevision;
	UINT32						CreatorID;
	UINT32						CreatorRevision;
} SDT_HEADER;

typedef struct ATTRIB_PACKED {
	SDT_HEADER					Header;
	UINT32						Table[];
} RSDT;

typedef struct ATTRIB_PACKED {
	SDT_HEADER					Header;
	UINT64						Table[];
} XSDT;

typedef struct ATTRIB_PACKED {
	UINT8						AddressSpace;
	UINT8						BitWidth;
	UINT8						BitOffset;
	UINT8						AccessSize;
	UINT64						Address;
} GENERIC_ADDRESS_STRUCTURE;

typedef struct ATTRIB_PACKED {
	SDT_HEADER					Header;
	UINT32						FirmwareControl;
	UINT32						DSDTAddress;
	UINT8						Reserved1;
	UINT8						PreferredPowerManagementProfile;
	UINT16						SCIInterrupt;
	UINT32						SMICommand;
	UINT8						ACPIEnable;
	UINT8						ACPIDisable;
	UINT8						S4BIOSReq;
	UINT8						PStateControl;
	UINT32						PM1AEventBlock;
	UINT32						PM1BEventBlock;
	UINT32						PM1AControlBlock;
	UINT32						PM1BControlBlock;
	UINT32						PM2ControlBlock;
	UINT32						PMTimerBlock;
	UINT32						GPE0Block;
	UINT32						GPE1Block;
	UINT8						PM1EventLength;
	UINT8						PM1ControlLength;
	UINT8						PM2ControlLength;
	UINT8						PMTimerLength;
	UINT8						GPE0Length;
	UINT8						GPE1Length;
	UINT8						GPE1Base;
	UINT8						CStateControl;
	UINT16						WorstC2Latency;
	UINT16						WorstC3Latency;
	UINT16						FlushSize;
	UINT16						FlushStride;
	UINT8						DutyOffset;
	UINT8						DutyWidth;
	UINT8						DayAlarm;
	UINT8						MonthAlarm;
	UINT8						Century;
	UINT16						BootArchitectureFlags;
	UINT8						Reserved2;
	UINT32						Flags;
	GENERIC_ADDRESS_STRUCTURE	ResetRegister;
	UINT8						ResetValue;
	UINT16						Reserved3;
	UINT8						MinorVersion;								//	ACPI minor version, major located in Header.Revision
	UINT64						ExtendedFirmwareControl;
	UINT64						ExtendedDSDTAddress;
	GENERIC_ADDRESS_STRUCTURE	ExtendedPM1AEventBlock;
	GENERIC_ADDRESS_STRUCTURE	ExtendedPM1BEventBlock;
	GENERIC_ADDRESS_STRUCTURE	ExtendedPM1AControlBlock;
	GENERIC_ADDRESS_STRUCTURE	ExtendedPM1BControlBlock;
	GENERIC_ADDRESS_STRUCTURE	ExtendedPM2ControlBlock;
	GENERIC_ADDRESS_STRUCTURE	ExtendedPMTimerBlock;
	GENERIC_ADDRESS_STRUCTURE	ExtendedGPE0Block;
	GENERIC_ADDRESS_STRUCTURE	ExtendedGPE1Block;
} FADT;

BOOL InitACPI(VOID);
BOOL CheckRSDP(VOID* rsdp);
BOOL CheckSDT(VOID* sdt, const CHAR* signature);
BOOL ACPIInitialized(VOID);
UINT8 GetACPIMajorVersion(VOID);
UINT8 GetACPIMinorVersion(VOID);
UINT16 GetBootArchitectureFlags(VOID);
BOOL ACPIPowerOff(VOID);

#endif