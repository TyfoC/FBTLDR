#include <terminal.h>

static CHAR DigitBuffer[MAX_INT_TO_STRING_BUFFER_LENGTH];
static const CHAR HorizontalTab[] = "    ", VerticalTab[] = "\n\n\n\n";
static CHAR* TerminalBuffer = (CHAR*)0xb8000;
static SIZE_T CursorOffset = 0;

static BIOS_COLOR InputStringKeyHandlerColor;
VOID InputStringKeyHandler(CHAR readChar);

STRING_POSITION GetCursorPosition(VOID) {
	STRING_POSITION position = {
		CursorOffset / TERMINAL_WIDTH_SIZE,
		(CursorOffset % TERMINAL_WIDTH_SIZE) >> 1
	};

	return position;
}

BOOL SetCursorPosition(const STRING_POSITION position) {
	if (position.Column >= TERMINAL_WIDTH || position.Line >= TERMINAL_HEIGHT) return FALSE;
	CursorOffset = (position.Column << 1) + position.Line * TERMINAL_WIDTH_SIZE;
	return TRUE;
}

SIZE_T GetCursorOffset(VOID) {
	return CursorOffset;
}

BOOL SetCursorOffset(SIZE_T offset) {
	if (offset >= TERMINAL_SIZE) return FALSE;
	CursorOffset = offset & 0xfffffffe;
	return TRUE;
}

VOID FixCursorPosition(VOID) {
	if (CursorOffset >= TERMINAL_SIZE) {
		CursorOffset -= TERMINAL_WIDTH_SIZE;
		CopyMemory(TerminalBuffer, &TerminalBuffer[TERMINAL_WIDTH_SIZE], TERMINAL_SIZE - TERMINAL_WIDTH_SIZE);
		FillMemory((VOID*)&TerminalBuffer[TERMINAL_SIZE - TERMINAL_WIDTH_SIZE], '\0', TERMINAL_WIDTH_SIZE);
	}
}

UINT8* GetTerminalBuffer(VOID) {
	return TerminalBuffer;
}

VOID PutChar(CHAR character, BIOS_COLOR characterColor) {
	STRING_POSITION tmpStrPos = { 0, 0 };
	if (character == '\t') PutString(HorizontalTab, characterColor);
	else if (character == '\v') PutString(VerticalTab, characterColor);
	else if (character == '\r') {
		tmpStrPos.Line = CursorOffset / TERMINAL_WIDTH_SIZE;
		SetCursorPosition(tmpStrPos);
	}
	else if (character == '\n') {
		if (CursorOffset + TERMINAL_WIDTH_SIZE < TERMINAL_SIZE) CursorOffset += TERMINAL_WIDTH_SIZE;
		else {
			CopyMemory(TerminalBuffer, &TerminalBuffer[TERMINAL_WIDTH_SIZE], TERMINAL_SIZE - TERMINAL_WIDTH_SIZE);
			FillMemory((VOID*)&TerminalBuffer[TERMINAL_SIZE - TERMINAL_WIDTH_SIZE], '\0', TERMINAL_WIDTH_SIZE);
		}
	}
	else if (character == '\b') {
		if (CursorOffset >= 2) CursorOffset -= 2;
	}
	else {
		FixCursorPosition();
		TerminalBuffer[CursorOffset++] = character;
		TerminalBuffer[CursorOffset++] = characterColor;
	}
}

VOID PutString(const CHAR* str, BIOS_COLOR strColor) {
	for (SIZE_T i = 0; str[i]; i++) PutChar(str[i], strColor);
}

/*
	%% - %
	%c - char from args
	%s - string from args
	%d - signed decimal value
	%u - unsigned decimal value
	%[x/o/q/b][s/u] - signed/unsigned hexadecimal/octal/quaternary/binary value
	%a - set font & background color from args
	%r - set current color to defaultColor
*/
VOID PrintFormatted(const CHAR* format, BIOS_COLOR defaultColor, ...) {
	UINT8 radix;
	BIOS_COLOR currentColor = defaultColor;
	SIZE_T* ptrArg = (SIZE_T*)((SIZE_T)&format + (sizeof(VOID*) << 1));
	for (SIZE_T i = 0; format[i]; i++) {
		if (format[i] == '%') {
			i += 1;

			if (format[i] == '%') PutChar('%', currentColor);
			else if (format[i] == 'c') PutChar((CHAR)*ptrArg++, currentColor);
			else if (format[i] == 's') PutString((CHAR*)*ptrArg++, currentColor);
			else if (format[i] == 'd') {
				IntToString((PTRDIFF_T)*ptrArg++, 10, DigitBuffer);
				PutString(DigitBuffer, currentColor);
			}
			else if (format[i] == 'u') {
				UintToString((SIZE_T)*ptrArg++, 10, DigitBuffer);
				PutString(DigitBuffer, currentColor);
			}
			else if (format[i] == 'x' || format[i] == 'o' || format[i] == 'q' || format[i] == 'b') {
				if (format[i] == 'x') radix = 16;
				else if (format[i] == 'o') radix = 8;
				else if (format[i] == 'q') radix = 4;
				else if (format[i] == 'b') radix = 2;

				i += 1;
				if (format[i] == 's') IntToString((PTRDIFF_T)*ptrArg++, radix, DigitBuffer);
				else if (format[i] == 'u') UintToString((SIZE_T)*ptrArg++, radix, DigitBuffer);
				PutString(DigitBuffer, currentColor);
			}
			else if (format[i] == 'a') currentColor = (BIOS_COLOR)*ptrArg++;
			else if (format[i] == 'r') currentColor = defaultColor;
		}
		else PutChar(format[i], currentColor);
	}
}

CHAR GetTerminalCharByOffset(VOID) {
	return TerminalBuffer[CursorOffset];
}

BIOS_COLOR GetTerminalCharColorByOffset(VOID) {
	return TerminalBuffer[CursorOffset + 1];
}

CHAR InputChar(BIOS_COLOR color) {
	BOOL entered = FALSE;
	CHAR character, tmp;
	do {
		tmp = PS2KbdReadChar();
		if (tmp == '\n') {
			if (entered) {
				PutString("\r\n", color);
				break;
			}
		}
		else if (tmp == '\b' && entered) {
			entered = FALSE;
			if (CursorOffset) {
				CursorOffset -= 2;
				PutChar(' ', color);
				CursorOffset -= 2;
			}
		}
		else {
			if (entered && CursorOffset) CursorOffset -= 2;
			character = tmp;
			entered = TRUE;
			PutChar(character, color);
		}

	} while (TRUE);

	return character;
}

VOID InputString(BIOS_COLOR color, CHAR* result, SIZE_T maxLength) {
	InputStringKeyHandlerColor = color;
	PS2KbdReadString(result, maxLength, InputStringKeyHandler);
}

VOID InputStringKeyHandler(CHAR readChar) {
	if (readChar == '\b') {
		if (CursorOffset) {
			CursorOffset -= 2;
			PutChar(' ', InputStringKeyHandlerColor);
			CursorOffset -= 2;
		}
	}
	else if (readChar == '\n') PutString("\r\n", InputStringKeyHandlerColor);
	else PutChar(readChar, InputStringKeyHandlerColor);
}