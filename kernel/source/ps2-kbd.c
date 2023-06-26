#include <ps2-kbd.h>

static UINT16 KbdType = PS2_DEV_TYPE_UNDEFINED;
static SIZE_T KbdIndex = NPOS;
static BOOL ShiftPressed = FALSE;
static BOOL CapsLockActivated = FALSE;

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
	'\'',		'`',		0x00,		'\\',
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
	0x00,		0x00,		0x00,		0x00,
	
	//	Shift, no caps
	0x00,		0x00,		'!',		'@',
	'#',		'$',		'%',		'^',
	'&',		'*',		'(',		')',
	'_',		'+',		'\b',		'\t',
	'Q',		'W',		'E',		'R',
	'T',		'Y',		'U',		'I',
	'O',		'P',		'{',		'}',
	'\n',		0x00,		'A',		'S',
	'D',		'F',		'G',		'H',
	'J',		'K',		'L',		':',
	'\"',		'~',		0x00,		'\\',
	'Z',		'X',		'C',		'V',
	'B',		'N',		'M',		'<',
	'>',		'?',		0x00,		'*',
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
	0x00,		0x00,		0x00,		0x00,

	// No shift, caps
	0x00,		0x00,		'1',		'2',
	'3',		'4',		'5',		'6',
	'7',		'8',		'9',		'0',
	'-',		'=',		'\b',		'\t',
	'Q',		'W',		'E',		'R',
	'T',		'Y',		'U',		'I',
	'O',		'P',		'[',		']',
	'\n',		0x00,		'A',		'S',
	'D',		'F',		'G',		'H',
	'J',		'K',		'L',		';',
	'\'',		'`',		0x00,		'\\',
	'Z',		'X',		'C',		'V',
	'B',		'N',		'M',		',',
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
	0x00,		0x00,		0x00,		0x00,

	//	Shift, caps
	0x00,		0x00,		'!',		'@',
	'#',		'$',		'%',		'^',
	'&',		'*',		'(',		')',
	'_',		'+',		'\b',		'\t',
	'q',		'w',		'e',		'r',
	't',		'y',		'u',		'i',
	'o',		'p',		'{',		'}',
	'\n',		0x00,		'a',		's',
	'd',		'f',		'g',		'h',
	'j',		'k',		'l',		':',
	'\"',		'~',		0x00,		'\\',
	'z',		'x',		'c',		'v',
	'b',		'n',		'm',		'<',
	'>',		'?',		0x00,		'*',
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
	0x00,		0x00,		0x00,		0x00,
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

KBD_SCAN_DATA PS2KbdReadKey(BOOL handleSpecialKeys) {
	KBD_SCAN_DATA result;

	if (handleSpecialKeys) {
		result.ScanCode = PS2DevBufferPopFirstByte(KbdIndex);
		result.Released = result.ScanCode & 0x80;
	}
	else {
		do {
			result.ScanCode = PS2DevBufferPopFirstByte(KbdIndex);
			result.Released = result.ScanCode & 0x80;

			if (result.ScanCode == PS2_KBD_SCS1_EXTENDED_SCAN_CODE) {
				result.ScanCode <<= 8;
				result.ScanCode |= PS2DevBufferPopFirstByte(KbdIndex);
				result.Released = result.ScanCode & 0x90;
				break;
			}
			else if (
				(result.ScanCode & 0x7F) == PS2_KBD_SCS1_LEFT_SHIFT ||
				(result.ScanCode & 0x7F) == PS2_KBD_SCS1_RIGHT_SHIFT
			) ShiftPressed = !ShiftPressed;
			else if (result.ScanCode == PS2_KBD_SCS1_CAPS_LOCK) {
				if (!result.Released) CapsLockActivated = !CapsLockActivated;
			}
			else break;
		} while (TRUE);
	}

	result.Character = FirstScanCodeSet[(result.ScanCode & 0x7F) + (((ShiftPressed & 1) | ((CapsLockActivated & 1) << 1)) << 7)];
	return result;
}

CHAR PS2KbdReadChar(VOID) {
	KBD_SCAN_DATA scanData;
	SIZE_T scanCode;
	do {
		scanData = PS2KbdReadKey(FALSE);
		scanCode = scanData.ScanCode & 0x7F;
		if (
			!scanData.Released &&
			scanCode != PS2_KBD_SCS1_ESCAPE &&
			scanCode != PS2_KBD_SCS1_LEFT_CTRL &&
			scanCode != PS2_KBD_SCS1_RIGHT_CTRL &&
			scanCode != PS2_KBD_SCS1_LEFT_ALT &&
			scanCode != PS2_KBD_SCS1_RIGHT_ALT &&
			scanCode != PS2_KBD_SCS1_NUM_LOCK &&
			scanCode != PS2_KBD_SCS1_SCROLL_LOCK
		) break;
	}
	while (TRUE);
	return scanData.Character;
}

VOID PS2KbdReadString(CHAR* result, SIZE_T maxLength, VOID (*KeyHandler)(CHAR readChar)) {
	if (!maxLength) maxLength = NPOS;
	SIZE_T length = 0;
	CHAR character = 0;

	if (KeyHandler) {
		while (TRUE) {
			character = PS2KbdReadChar();
			if (character == '\b' && length) {
				result[length--] = ' ';
				KeyHandler(character);
			}
			else if (character == '\n') {
				result[length] = 0;
				KeyHandler(character);
				break;
			}
			else if (character != '\b' && (SIZE_T)length < maxLength) {
				result[length++] = character;
				KeyHandler(character);
			}
		}
	}
	else {
		while (TRUE) {
			character = PS2KbdReadChar();
			if (character == '\b' && length) result[length--] = ' ';
			else if (character == '\n') {
				result[length] = 0;
				break;
			}
			else if (character != '\b' && (SIZE_T)length < maxLength) result[length++] = character;
		}
	}
}