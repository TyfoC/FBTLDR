#pragma once
#ifndef INT_UTILS
#define INT_UTILS

#include "typedefs.h"

#define MAX_INT_TO_STRING_BUFFER_LENGTH	66

BOOL IntToString(PTRDIFF_T value, UINT8 radix, CHAR* buffer);
BOOL UintToString(SIZE_T value, UINT8 radix, CHAR* buffer);

#endif