#include <fpu.h>

VOID InstallFPU(VOID) {
	SIZE_T cr4Value = GetCR4Value() | 0x200;
	ASM("mov %0, %%cr4" :: "r"(cr4Value));
	WriteFPUControlWord(0x37F);
}

VOID WriteFPUControlWord(UINT16 value) {
	ASM("fldcw %0"::"m"(value));
}