#pragma once
#ifndef CPU
#define CPU

#include "gdt.h"

void SwitchToProtectedMode(
	const GdtRegister* gdtReg,
	uint32_t returnAddress,
	uint16_t codeSegOff,
	uint16_t dataSegOff,
	uint32_t stackTopAddress,
	uint32_t dataAddress
);

#endif