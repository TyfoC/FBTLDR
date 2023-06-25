#pragma once
#ifndef ISRS
#define ISRS

#include "cpu-desc-tables.h"
#include "asm-utils.h"
#include "pic.h"
#include "terminal.h"

#define HARDWARE_INT_HANDLERS_COUNT		0x10

typedef struct ATTRIB(__packed__) INT_HANDLER_REGISTERS {
	UINT32	GS;
	UINT32	FS;
	UINT32	ES;
	UINT32	DS;
	UINT32	EDI;
	UINT32	ESI;
	UINT32	EBP;
	UINT32	UselessESP;
	UINT32	EBX;
	UINT32	EDX;
	UINT32	ECX;
	UINT32	EAX;
	UINT32	InterruptIndex;
	UINT32	ErrorCode;
	UINT32	EIP;
	UINT32	CS;
	UINT32	EFLAGS;
	UINT32	UserESP;
	UINT32	SS;
} INT_HANDLER_REGISTERS;

typedef VOID (*INT_HANDLER)(INT_HANDLER_REGISTERS* registers);

VOID InstallSoftwareIntHandler(SIZE_T intIndex, INT_HANDLER intHandler);
VOID InstallHardwareIntHandler(SIZE_T intIndex, INT_HANDLER intHandler);
VOID UninstallSoftwareIntHandler(SIZE_T intIndex);
VOID UninstallHardwareIntHandler(SIZE_T intIndex);
VOID GetSoftwareIntHandler(SIZE_T intIndex, INT_HANDLER* intHandler);
VOID GetHardwareIntHandler(SIZE_T intIndex, INT_HANDLER* intHandler);
VOID InitSoftwareIntHandlers(IDT_ENTRY* idt, UINT16 codeSegValue);
VOID InitHardwareIntHandlers(IDT_ENTRY* idt, UINT16 codeSegValue);

#endif