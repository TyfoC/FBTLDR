#pragma once
#ifndef TERMINAL
#define TERMINAL

#include "cstring.h"

void PutChar(char value);
void PutString(const char* str);
void PrintFormatted(const char* fmt, ...);

#endif