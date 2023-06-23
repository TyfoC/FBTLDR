#include <cstring.h>

static const char IntToStringDigits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

size_t GetStringLength(const char* str) {
	char* ptr = (char*)&str[0];
	while (*ptr) ++ptr;
	return (size_t)ptr - (size_t)str;
}

bool_t IntToString(ptrdiff_t value, uint8_t base, char* result) {
	if (base < 2 || base > 16) return FALSE;

	char* ptr = result;
	if (value < 0) {
		result[0] = '-';
		ptr = &result[1];
		value = -value;
	}

	size_t valueLength = 1;
	ptrdiff_t valueCopy = value;
	while (valueCopy /= base) ++valueLength;

	ptr[valueLength] = 0;
	char* invPtr = (char*)&ptr[valueLength - 1];
	while (valueLength--) {
		*invPtr-- = IntToStringDigits[value % base];
		value /= base;
	}

	return TRUE;
}

bool_t UintToString(size_t value, uint8_t base, char* result) {
	if (base < 2 || base > 16) return FALSE;

	size_t valueLength = 1, valueCopy = value;
	while (valueCopy /= base) ++valueLength;
	result[valueLength] = 0;

	char* ptr = (char*)&result[valueLength - 1];
	while (valueLength--) {
		*ptr-- = IntToStringDigits[value % base];
		value /= base;
	}

	return TRUE;
}