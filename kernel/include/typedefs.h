#pragma once
#ifndef TYPEDEFS
#define TYPEDEFS

#define ALIGN_UP($value, $alignment)	((($value) + ($alignment) - 1) & ~(($alignment) - 1))
#define ALIGN_DOWN($value, $alignment)	(($value) & ~(($alignment) - 1))
#define ATTRIB(...)						__attribute__((__VA_ARGS__))
#define ASM(...)						__asm__ __volatile__(__VA_ARGS__)
#define STOP(...)						ASM("jmp ."::__VA_ARGS__)
#define HLT(...)				ASM("cli; hlt; jmp ."::__VA_ARGS__)

#define FALSE							0
#define TRUE							1

#define NPOS							((SIZE_T)-1)

typedef char INT8, CHAR;
typedef unsigned char UINT8, UCHAR;
typedef short INT16, SHORT;
typedef unsigned short UINT16, USHORT;
typedef int INT32, INT;
typedef unsigned int UINT32, UINT;
typedef long long INT64, LONGLONG;
typedef unsigned long long UINT64, ULONGLONG;
typedef __PTRDIFF_TYPE__ PTRDIFF_T, BOOL;
typedef __SIZE_TYPE__ SIZE_T;
typedef void VOID;

#endif