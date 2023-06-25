#include "acpi.h"

static BOOL SCIEnabled = FALSE;
static UINT8 MajorVersion;
static UINT8 MinorVersion;
static UINT32 SMICommand;
static UINT8 Enable;
static UINT8 Disable;
static UINT32 PM1AControl;
static UINT32 PM1BControl;
static UINT8 PM1ControlLength;
static UINT16 BootArchFlags;
static UINT16 SleepTypeA;
static UINT16 SleepTypeB;
static UINT16 SleepEnable;

BOOL InitACPI(VOID) {
	RSDP2* rsdp = (RSDP2*)(*((UINT16*)0x0000040E) * 0x10 & 0x000FFFFF);
	SIZE_T searchBound = (SIZE_T)rsdp + 0x400;

	for (; (SIZE_T)rsdp < searchBound;) {
		if (CheckRSDP(rsdp)) break;
		rsdp = (RSDP2*)((SIZE_T)rsdp + RSDP_SIGNATURE_SIZE);
	}

	if (!CheckRSDP(rsdp)) {
		rsdp = (RSDP2*)0x000E0000;
		searchBound = 0x00100000;

		for (; (SIZE_T)rsdp < searchBound;) {
			if (CheckRSDP(rsdp)) break;
			rsdp = (RSDP2*)((SIZE_T)rsdp + RSDP_SIGNATURE_SIZE);
		}

		if (!CheckRSDP(rsdp)) return FALSE;
	}

	RSDT* rsdt;
	SIZE_T sdtCount, ptrShift;
	if (!rsdp->Revision) {
		rsdt = (RSDT*)rsdp->RSDTAddress;
		sdtCount = rsdt->Header.Length - sizeof(SDT_HEADER);
		ptrShift = sizeof(UINT32);
		if (!CheckSDT(&rsdt->Header, RSDT_SIGNATURE)) return FALSE;
	}
	else {
		if (rsdp->XSDTAddress >= 0x0000000100000000) {
			if (!rsdp->RSDTAddress) return FALSE;
			rsdt = (RSDT*)rsdp->RSDTAddress;
			ptrShift = sizeof(UINT32);
			if (!CheckSDT(&rsdt->Header, RSDT_SIGNATURE)) return FALSE;
		}
		else {
			rsdt = (RSDT*)((SIZE_T)rsdp->XSDTAddress);
			ptrShift = sizeof(UINT64);
			if (!CheckSDT(&rsdt->Header, XSDT_SIGNATURE)) return FALSE;
		}
	}

	void* sdtPointer = (void*)((SIZE_T)rsdt + sizeof(SDT_HEADER)), *dsdt;
	SDT_HEADER* sdtHeader;
	FADT* fadt;
	for (SIZE_T i = 0; i < sdtCount; i++, sdtPointer = (void*)((SIZE_T)sdtPointer + ptrShift)) {
		if (ptrShift == sizeof(UINT64) && *((UINT64*)sdtPointer) >= 0x0000000100000000) continue;
		
		sdtHeader = (SDT_HEADER*)*(SIZE_T*)sdtPointer;

		if (CheckSDT(sdtHeader, FADT_SIGNATURE)) {
			fadt = (FADT*)sdtHeader;

			MajorVersion = fadt->Header.Revision;
			MinorVersion = fadt->MinorVersion;

			if (fadt->ExtendedDSDTAddress && fadt->ExtendedDSDTAddress < 0x0000000100000000) {
				dsdt = (void*)(SIZE_T)(fadt->ExtendedDSDTAddress);
			}
			else dsdt = (void*)fadt->DSDTAddress;

			if (!CheckSDT((SDT_HEADER*)dsdt, DSDT_SIGNATURE)) return FALSE;

			UINT8* s5Obj = (UINT8*)((SIZE_T)dsdt + sizeof(SDT_HEADER));
			SIZE_T dsdtLength = ((SDT_HEADER*)dsdt)->Length - sizeof(SDT_HEADER);
			
			while (dsdtLength--) {
				if (CompareMemory(s5Obj, DSDT_S5_SIGNATURE, 4)) break;
				s5Obj++;
			}

			if (!dsdtLength) return FALSE;

			if ((*(s5Obj - 1) == 0x08 || (*(s5Obj - 2) == 0x08 && *(s5Obj - 1) == '\\')) && *(s5Obj + 4) == 0x12) {
				s5Obj += 5;
				s5Obj += ((*s5Obj & 0xc0) >> 6) + 2;

				if (*s5Obj == 0x0a) ++s5Obj;
				SleepTypeA = *(s5Obj) << 10;

				SMICommand = fadt->SMICommand;

				Enable = fadt->ACPIEnable;
				Disable = fadt->ACPIDisable;

				PM1AControl = fadt->PM1AControlBlock;
				PM1BControl = fadt->PM1BControlBlock;

				PM1ControlLength = fadt->PM1ControlLength;

				BootArchFlags = fadt->BootArchitectureFlags;

				SleepEnable = 0x2000;
				SCIEnabled = TRUE;

				// enabling acpi
				if (!(InWord((UINT16)PM1AControl) & SCIEnabled)) {
					if (SMICommand && Enable) {
						OutByte((UINT16)SMICommand, Enable);
						SIZE_T j = 0;
						for (; j < 300; j++) {
							if ((InWord((UINT16)PM1AControl) & SCIEnabled) == 1) break;
							Sleep(10);
						}

						if (PM1BControl) {
							for (; j < 300; j++) {
								if ((InWord((UINT16)PM1BControl) & SCIEnabled) == 1) break;
								Sleep(10);
							}
						}

						if (j >= 300) return FALSE;
					}
					else return FALSE;
				}
			}
		}
	}

	return TRUE;
}

BOOL CheckRSDP(VOID* rsdp) {
	RSDP2* ptr = (RSDP2*)rsdp;
	if (!CompareMemory(ptr, RSDP_SIGNATURE, 8)) return FALSE;

	UINT8 value = 0;
	SIZE_T checkLen = ptr->Revision ? ptr->Length : sizeof(RSDP1);
	for (SIZE_T i = 0; i < checkLen; i++) value += ((UINT8*)ptr)[i];
	return value == 0;
}

BOOL CheckSDT(VOID* sdt, const CHAR* signature) {
	SDT_HEADER* hdr = (SDT_HEADER*)sdt;
	if (!CompareMemory(hdr, signature, GetStringLength(signature))) return FALSE;

	UINT8 value = 0;
	for (SIZE_T i = 0; i < hdr->Length; i++) value += ((UINT8*)hdr)[i];
	return value == 0;
}

BOOL ACPIInitialized(VOID) {
	return SCIEnabled;
}

UINT8 GetACPIMajorVersion(VOID) {
	return MajorVersion;
}

UINT8 GetACPIMinorVersion(VOID) {
	return MinorVersion;
}

UINT16 GetBootArchitectureFlags(VOID) {
	return BootArchFlags;
}

BOOL ACPIPowerOff(VOID) {
	if (SCIEnabled && InWord((UINT16)PM1AControl) & SCIEnabled) {
		OutWord((UINT16)PM1AControl, SleepTypeA | SleepEnable);
		if (PM1BControl) OutWord((UINT16)PM1BControl, SleepTypeB | SleepEnable);
		return TRUE;
	}
	
	return FALSE;
}