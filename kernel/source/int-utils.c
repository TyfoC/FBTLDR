#include <int-utils.h>

static const CHAR Digits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

BOOL IntToString(PTRDIFF_T value, UINT8 radix, CHAR* buffer) {
	if (radix < 2 || radix > 16) return FALSE;

	CHAR* ptr = buffer;
	if (value < 0) {
		buffer[0] = '-';
		ptr = &buffer[1];
		value = -value;
	}

	SIZE_T valueLength = 1;
	PTRDIFF_T valueCopy = value;
	while (valueCopy /= radix) ++valueLength;

	ptr[valueLength] = 0;
	CHAR* invPtr = (CHAR*)&ptr[valueLength - 1];
	while (valueLength--) {
		*invPtr-- = Digits[value % radix];
		value /= radix;
	}

	return TRUE;
}

BOOL UintToString(SIZE_T value, UINT8 radix, CHAR* buffer) {
	if (radix < 2 || radix > 16) return FALSE;

	SIZE_T valueLength = 1, valueCopy = value;
	while (valueCopy /= radix) ++valueLength;
	buffer[valueLength] = 0;

	CHAR* invPtr = (CHAR*)&buffer[valueLength - 1];
	while (valueLength--) {
		*invPtr-- = Digits[value % radix];
		value /= radix;
	}

	return TRUE;
}