#pragma once
#ifndef PS2_MOUSE
#define PS2_MOUSE

#include "ps2-dev.h"

#define PS2_MOUSE_BUFFER_Y_OFFSET			0x00
#define PS2_MOUSE_BUFFER_X_OFFSET			0x01
#define PS2_MOUSE_BUFFER_FLAGS_OFFSET		0x02
#define PS2_MOUSE_WIDTH						10000
#define PS2_MOUSE_HEIGHT					10000
#define PS2_MOUSE_MAX_X						(PS2_MOUSE_WIDTH - 1)
#define PS2_MOUSE_MAX_Y						(PS2_MOUSE_HEIGHT - 1)

#define PS2_MOUSE_COMMAND_SET_SAMPLE_RATE	0xF3

BOOL InstallPS2Mouse(VOID);
SIZE_T PS2MouseGetWorldX(VOID);
SIZE_T PS2MouseGetWorldY(VOID);
SIZE_T PS2MouseGetX(SIZE_T screenWidth, SIZE_T pixelWidth);
SIZE_T PS2MouseGetY(SIZE_T screenHeight, SIZE_T pixelHeight);
BOOL PS2MouseSetSampleRate(SIZE_T value);

#endif