#include <math-utils.h>

PTRDIFF_T IntAlignUp(PTRDIFF_T value, SIZE_T alignment) {
	PTRDIFF_T remainder = value % alignment;
	return remainder ? value + (PTRDIFF_T)(alignment - remainder) : value;
}

SIZE_T UintAlignUp(SIZE_T value, SIZE_T alignment) {
	SIZE_T remainder = value % alignment;
	return remainder ? value + (alignment - remainder) : value;
}