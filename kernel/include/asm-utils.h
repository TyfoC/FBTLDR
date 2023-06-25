#pragma once
#ifndef ASM_UTILS
#define ASM_UTILS

#include "typedefs.h"

#define ENABLE_HARDWARE_INTERRUPTS()	ASM("sti")
#define DISABLE_HARDWARE_INTERRUPTS()	ASM("cli")

UINT16 GetCodeSegValue(VOID);
UINT16 GetDataSegValue(VOID);
UINT32 GetCR0Value(VOID);
UINT32 GetCR2Value(VOID);
UINT32 GetCR4Value(VOID);
VOID LoadGDTRegister(const void* gdtReg);
VOID LoadIDTRegister(const void* idtReg);
VOID IOWait(VOID);

#endif