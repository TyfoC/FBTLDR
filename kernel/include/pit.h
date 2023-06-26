#pragma once
#ifndef PIT
#define PIT

#include "cpu-int-handlers.h"

#define PIT_PORT_FIRST_COUNTER	0x40
#define PIT_PORT_COMMAND		0x43

#define PIT_BYTE_ACCESS_LOW		0x10
#define PIT_BYTE_ACCESS_HIGH	0x20
#define PIT_BYTE_ACCESS_BOTH	0x30

#define PIT_MODE_SQUARE_WAVE	0x06

#define PIT_IRQ_INDEX			0x00
#define PIT_HARDWARE_FREQUENCY	1193181
#define PIT_SOFTWARE_FREQUENCY	1000

//	PIT_SOFTWARE_FREQUENCY - 1 ms/tick
VOID InstallPIT(SIZE_T frequency);
VOID SetPITFrequency(SIZE_T frequency);
VOID Sleep(UINT64 ticksCount);
SIZE_T GetTicksCount(VOID);
UINT64 GetTicksCount64(VOID);

#endif