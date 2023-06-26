#include <ps2-kbd.h>

static UINT16 KbdType = PS2_DEV_TYPE_UNDEFINED;
static SIZE_T KbdIndex = NPOS;

static const CHAR FirstScanCodeSet[] = {
	//	No shift, no caps
	0x00,		0x00,		'1',		'2',
	'3',		'4',		'5',		'6',
	'7',		'8',		'9',		'0',
	'-',		'=',		'\b',		'\t',
	'q',		'w',		'e',		'r',
	't',		'y',		'u',		'i',
	'o',		'p',		'[',		']',
	'\n',		0x00,		'a',		's',
	'd',		'f',		'g',		'h',
	'j',		'k',		'l',		';',
	'\'',		'`',		0x00,		'\'',
	'z',		'x',		'c',		'v',
	'b',		'n',		'm',		',',
	'.',		'/',		0x00,		'*',
	0x00,		' ',		0x00,		0x00,
	0x00,		0x00,		0x00,		0x00,
	0x00,		0x00,		0x00,		0x00,
	0x00,		0x00,		0x00,		'7',
	'8',		'9',		'-',		'4',
	'5',		'6',		'+',		'1',
	'2',		'3',		'0',		'.',
	0x00,		0x00,		0x00,		0x00,
	0x00,		0x00,		0x00,		0x00,
	0x00,		0x00,		0x00,		0x00,
	0x00,		0x00,		0x00,		0x00,
	0x00,		0x00,		0x00,		0x00,
	0x00,		0x00,		0x00,		0x00,
	0x00,		0x00,		0x00,		0x00,
	0x00,		0x00,		0x00,		0x00,
	0x00,		0x00,		0x00,		0x00,
	0x00,		0x00,		0x00,		0x00,
	0x00,		0x00,		0x00,		0x00
};

BOOL InstallPS2Kbd(VOID) {
	if (!PS2DevsInitialized()) if (!PS2DevsInit()) return FALSE;

	KbdType = PS2DevGetKbdType();
	KbdIndex = PS2DevGetKbdIndex();

	if (KbdIndex != NPOS) PS2KbdSetScanCodeSet(1);
	return KbdIndex != NPOS;
}

UINT16 PS2KbdGetType(VOID) {
	return KbdType;
}

/*
	Returns:
		1 - first
		2 - second
		3 - third
*/
UINT8 PS2KbdGetScanCodeSet(VOID) {
	if (KbdIndex == NPOS) return PS2_KBD_UNDEFINED_SCAN_CODE_SET;

	UINT8 answer = PS2DevSendByte(KbdIndex, PS2_KBD_COMMAND_GET_SET_SCAN_CODE_SET, FALSE);
	if (answer != PS2_DEV_ANSWER_ACK) return PS2_KBD_UNDEFINED_SCAN_CODE_SET;

	answer = PS2DevSendByte(KbdIndex, PS2_KBD_SUB_COMMAND_GET_SCAN_CODE_SET, FALSE);
	if (answer != PS2_DEV_ANSWER_ACK) return PS2_KBD_UNDEFINED_SCAN_CODE_SET;

	answer = PS2DevBufferPopFirstByte(KbdIndex);
	if (answer == PS2_KBD_FIRST_SCAN_CODE_SET) return 1;
	else if (answer == PS2_KBD_SECOND_SCAN_CODE_SET) return 2;
	else if (answer == PS2_KBD_THIRD_SCAN_CODE_SET) return 3;
	return answer;
}

/*
	scanCodeSetNumber:
		1 - first
		2 - second
		3 - third
*/
BOOL PS2KbdSetScanCodeSet(SIZE_T scanCodeSetNumber) {
	if (KbdIndex == NPOS) return FALSE;

	UINT8 answer = PS2DevSendByte(KbdIndex, PS2_KBD_COMMAND_GET_SET_SCAN_CODE_SET, FALSE);
	if (answer != PS2_DEV_ANSWER_ACK) return FALSE;

	answer = PS2DevSendByte(KbdIndex, scanCodeSetNumber & 3, FALSE);
	return answer == PS2_DEV_ANSWER_ACK;
}

KBD_SCAN_DATA PS2KbdReadKey(VOID) {
	KBD_SCAN_DATA result;

	result.ScanCode = PS2DevBufferPopFirstByte(KbdIndex);
	result.Released = result.ScanCode & 0x80;
	result.Character = FirstScanCodeSet[result.ScanCode & 0x7F];

	return result;
}