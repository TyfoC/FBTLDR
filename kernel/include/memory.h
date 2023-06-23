#pragma once
#ifndef MEMORY
#define MEMORY

#include "typedefs.h"

VOID CopyMemory(VOID* destination, const VOID* source, SIZE_T count);
VOID FillMemory(VOID* destination, UINT8 value, SIZE_T count);

#endif