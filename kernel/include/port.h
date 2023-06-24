#pragma once
#ifndef PORT
#define PORT

#include "typedefs.h"

VOID OutByte(UINT16 portIndex, UINT8 value);
VOID OutWord(UINT16 portIndex, UINT16 value);
VOID OutLong(UINT16 portIndex, UINT32 value);
UINT8 InByte(UINT16 portIndex);
UINT16 InWord(UINT16 portIndex);
UINT16 InLong(UINT16 portIndex);

#endif