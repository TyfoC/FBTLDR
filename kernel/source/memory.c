#include <memory.h>

VOID CopyMemory(VOID* destination, const VOID* source, SIZE_T count) {
	UINT8* dstBuff = (UINT8*)destination;
	const UINT8* srcBuff = (UINT8*)source;
	for (SIZE_T i = 0; i < count; i++) dstBuff[i] = srcBuff[i];
}

VOID FillMemory(VOID* destination, UINT8 value, SIZE_T count) {
	UINT8* dstBuff = (UINT8*)destination;
	for (SIZE_T i = 0; i < count; i++) dstBuff[i] = value;
}