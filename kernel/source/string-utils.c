#include <string-utils.h>

SIZE_T GetStringLength(const CHAR* str) {
	SIZE_T result = 0;
	while (str[result]) result += 1;
	return result;
}

CHAR CharToLower(CHAR value) {
	if (value >= 'A' && value <= 'Z') value |= ' ';
	return value;
}

CHAR CharToUpper(CHAR value) {
	if (value >= 'a' && value <= 'z') value &= '_';
	return value;
}

BOOL StringToInt(const CHAR* str, UINT8 radix, PTRDIFF_T* buffer) {
	if (radix < 2 || radix > 36) return FALSE;

	BOOL isNegValue = FALSE;
	CHAR* ptr = (CHAR*)&str[0];
	if (ptr[0] == '+') ptr = &ptr[1];
	else if (ptr[0] == '-') {
		ptr = &ptr[1];
		isNegValue = TRUE;
	}

	CHAR lowered;
	for (SIZE_T i = 0; ptr[i]; i++) {
		lowered = CharToLower(ptr[i]);
		*buffer += lowered < 'a' ? lowered - '0' : lowered - 'a';
		*buffer <<= 1;
		*buffer *= 5;
	}

	*buffer >>= 1;
	*buffer /= 5;
	if (isNegValue) *buffer = -*buffer;
	return TRUE;
}

BOOL StringToUint(const CHAR* str, UINT8 radix, SIZE_T* buffer) {
	if (radix < 2 || radix > 36) return FALSE;

	CHAR* ptr = (CHAR*)&str[0];
	if (ptr[0] == '+') ptr = &ptr[1];
	else if (ptr[0] == '-') ptr = &ptr[1];

	CHAR lowered;
	for (SIZE_T i = 0; ptr[i]; i++) {
		lowered = CharToLower(ptr[i]);
		*buffer += lowered < 'a' ? lowered - '0' : lowered - 'a';
		*buffer <<= 1;
		*buffer *= 5;
	}

	*buffer >>= 1;
	*buffer /= 5;
	return TRUE;
}