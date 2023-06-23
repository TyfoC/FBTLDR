#pragma once
#ifndef STRING_UTILS
#define STRING_UTILS

#include "memory.h"

typedef struct STRING_POSITION {
	SIZE_T	Line;
	SIZE_T	Column;
} STRING_POSITION;

SIZE_T GetStringLength(const CHAR* str);
CHAR CharToLower(CHAR value);
CHAR CharToUpper(CHAR value);
BOOL StringToInt(const CHAR* str, UINT8 radix, PTRDIFF_T* buffer);
BOOL StringToUint(const CHAR* str, UINT8 radix, SIZE_T* buffer);

#endif