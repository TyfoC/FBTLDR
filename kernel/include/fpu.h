#pragma once
#ifndef FPU
#define FPU

#include "asm-utils.h"
#include "port.h"

VOID InstallFPU(VOID);
VOID WriteFPUControlWord(UINT16 value);

#endif