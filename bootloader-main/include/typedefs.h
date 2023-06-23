#pragma once
#ifndef TYPEDEFS
#define TYPEDEFS

#define ALIGN_UP($value, $alignment)	((($value) + ($alignment) - 1) & ~(($alignment) - 1))
#define ASM(...)						__asm__ __volatile__(__VA_ARGS__)
#define ATTRIB(...)						__attribute__((__VA_ARGS__))
#define STOP_EXECUTION(...)				ASM("cli; hlt; jmp ."::__VA_ARGS__)

#define FALSE				0
#define TRUE				1

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short in16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;
typedef __PTRDIFF_TYPE__ ptrdiff_t, bool_t;
typedef __SIZE_TYPE__ size_t;

#endif