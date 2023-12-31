#pragma once
#ifndef PS2_KBD
#define PS2_KBD

#include "ps2-dev.h"

#define PS2_KBD_COMMAND_GET_SET_SCAN_CODE_SET			0xF0
#define PS2_KBD_SUB_COMMAND_GET_SCAN_CODE_SET			0x00
#define PS2_KBD_SUB_COMMAND_SET_FIRST_SCAN_CODE_SET		0x01
#define PS2_KBD_SUB_COMMAND_SET_SECOND_SCAN_CODE_SET	0x02
#define PS2_KBD_SUB_COMMAND_SET_THIRD_SCAN_CODE_SET		0x03

#define PS2_KBD_FIRST_SCAN_CODE_SET						0x43
#define PS2_KBD_SECOND_SCAN_CODE_SET					0x41
#define PS2_KBD_THIRD_SCAN_CODE_SET						0x3F
#define PS2_KBD_UNDEFINED_SCAN_CODE_SET					0xFF

#define PS2_KBD_SCS1_EXTENDED_SCAN_CODE					0xE0
#define PS2_KBD_SCS1_LEFT_SHIFT							0x2A
#define PS2_KBD_SCS1_RIGHT_SHIFT						0x36
#define PS2_KBD_SCS1_CAPS_LOCK							0x3A
#define PS2_KBD_SCS1_ESCAPE								0x01
#define PS2_KBD_SCS1_LEFT_CTRL							0x1D
#define PS2_KBD_SCS1_RIGHT_CTRL							0xE090
#define PS2_KBD_SCS1_LEFT_ALT							0x38
#define PS2_KBD_SCS1_RIGHT_ALT							0xE0B8
#define PS2_KBD_SCS1_NUM_LOCK							0x45
#define PS2_KBD_SCS1_SCROLL_LOCK						0x46

typedef struct {
	UINT64	ScanCode;
	BOOL	Released;
	CHAR	Character;
} KBD_SCAN_DATA;

BOOL InstallPS2Kbd(VOID);
UINT16 PS2KbdGetType(VOID);
UINT8 PS2KbdGetScanCodeSet(VOID);
BOOL PS2KbdSetScanCodeSet(SIZE_T scanCodeSetNumber);
KBD_SCAN_DATA PS2KbdReadKey(BOOL handleSpecialKeys);
CHAR PS2KbdReadChar(VOID);
VOID PS2KbdReadString(CHAR* result, SIZE_T maxLength, VOID (*KeyHandler)(CHAR readChar));

#endif