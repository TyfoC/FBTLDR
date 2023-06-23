#pragma once
#ifndef CSTRING
#define CSTRING

#include "typedefs.h"

#define INT_TO_STR_BUFF_LENGTH	66

size_t GetStringLength(const char* str);
bool_t IntToString(ptrdiff_t value, uint8_t base, char* result);
bool_t UintToString(size_t value, uint8_t base, char* result);

#endif