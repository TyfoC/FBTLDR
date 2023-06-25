#include "pit.h"

static UINT64 TicksCount = 0;

extern VOID PITHandler(INT_HANDLER_REGISTERS*);

VOID InitPIT(SIZE_T frequency) {
	SetPITFrequency(frequency);
	InstallHardwareIntHandler(PIT_IRQ_INDEX, PITHandler);
	UnmaskPICIRQ(PIT_IRQ_INDEX);
}

VOID SetPITFrequency(SIZE_T frequency) {
	UINT16 divisor = PIT_HARDWARE_FREQUENCY / frequency;
	OutByte(PIT_PORT_COMMAND, PIT_MODE_SQUARE_WAVE | PIT_BYTE_ACCESS_BOTH);
	OutByte(PIT_PORT_FIRST_COUNTER, (UINT8)(divisor & 0xff));
	OutByte(PIT_PORT_FIRST_COUNTER, (UINT8)(divisor >> 8));
}

VOID Sleep(UINT64 ticksCount) {
	ticksCount += TicksCount;
	while (TicksCount < ticksCount) {}
}

SIZE_T GetTicksCount(VOID) {
	return (SIZE_T)TicksCount;
}

UINT64 GetTicksCount64(VOID) {
	return TicksCount;
}

VOID PITHandler(INT_HANDLER_REGISTERS*) {
	++TicksCount;
}