#pragma once
#ifndef TERMINAL
#define TERMINAL

#include "string-utils.h"
#include "int-utils.h"
#include "bios-colors.h"
#include "math-utils.h"
#include "ps2-kbd.h"

#define TERMINAL_WIDTH				0x50
#define TERMINAL_HEIGHT				0x19
#define TERMINAL_WIDTH_SIZE			0xA0
#define TERMINAL_SIZE				0xFA0

STRING_POSITION GetCursorPosition(VOID);
BOOL SetCursorPosition(const STRING_POSITION position);
SIZE_T GetCursorOffset(VOID);
BOOL SetCursorOffset(SIZE_T offset);
VOID FixCursorPosition(VOID);
UINT8* GetTerminalBuffer(VOID);
VOID PutChar(CHAR character, BIOS_COLOR characterColor);
VOID PutString(const CHAR* str, BIOS_COLOR strColor);
VOID PrintFormatted(const CHAR* format, BIOS_COLOR defaultColor, ...);
CHAR GetTerminalCharByOffset(VOID);
BIOS_COLOR GetTerminalCharColorByOffset(VOID);
CHAR InputChar(BIOS_COLOR color);
VOID InputString(BIOS_COLOR color, CHAR* result, SIZE_T maxLength);

#endif