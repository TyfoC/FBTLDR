#include "ps2-mouse.h"

static UINT16 MouseType = PS2_DEV_TYPE_UNDEFINED;
static SIZE_T MouseIndex = NPOS;
static UINT8* MouseBuffer = 0;
static SIZE_T* MouseBufferLen = 0;
static SIZE_T MouseCycle = 0;

static UINT8 MouseState = 0;
static SIZE_T MouseX = 0;
static SIZE_T MouseY = 0;

extern UINT8 FirstPS2DevBuffer[];
extern UINT8 SecondPS2DevBuffer[];
extern SIZE_T FirstPS2DevBufferLength;
extern SIZE_T SecondPS2DevBufferLength;

VOID PS2MouseHandler(UINT8 data);

BOOL InstallPS2Mouse(VOID) {
	if (!PS2DevsInitialized()) if (!PS2DevsInit()) return FALSE;

	MouseType = PS2DevGetMouseType();
	MouseIndex = PS2DevGetMouseIndex();
	if (MouseIndex == NPOS) return FALSE;

	if (MouseIndex == PS2_DEV_FIRST) {
		MouseBuffer = FirstPS2DevBuffer;
		MouseBufferLen = &FirstPS2DevBufferLength;
	}
	else {
		MouseBuffer = SecondPS2DevBuffer;
		MouseBufferLen = &SecondPS2DevBufferLength;
	}

	PS2DevSetIRQHandler(MouseIndex, PS2MouseHandler);
	return TRUE;
}

SIZE_T PS2MouseGetWorldX(VOID) {
	return MouseX;
}

SIZE_T PS2MouseGetWorldY(VOID) {
	return MouseY;
}

SIZE_T PS2MouseGetX(SIZE_T screenWidth, SIZE_T pixelWidth) {
	float divValue = (float)PS2_MOUSE_WIDTH / (float)screenWidth;
	float divX = MouseX / divValue;
	return (SIZE_T)divX >= screenWidth ? screenWidth - 1 : (SIZE_T)(divX / pixelWidth);
}

SIZE_T PS2MouseGetY(SIZE_T screenHeight, SIZE_T pixelHeight) {
	float divValue = (float)PS2_MOUSE_HEIGHT / (float)screenHeight;
	float divY = MouseY / divValue;
	return (SIZE_T)divY >= screenHeight ? screenHeight - 1 : (SIZE_T)(divY / pixelHeight);
}

BOOL PS2MouseSetSampleRate(SIZE_T value) {
	if (MouseIndex == NPOS) return FALSE;

	UINT8 answer = PS2DevSendByte(MouseIndex, PS2_MOUSE_COMMAND_SET_SAMPLE_RATE, FALSE);
	if (answer != PS2_DEV_ANSWER_ACK) return FALSE;

	answer = PS2DevSendByte(value, PS2_MOUSE_COMMAND_SET_SAMPLE_RATE, FALSE);
	return answer == PS2_DEV_ANSWER_ACK;
}

VOID PS2MouseHandler(UINT8 data) {
	if (MouseCycle == 0) {
		MouseState = data;
		MouseCycle = 1;
	}
	else if (MouseCycle == 1) {
		INT8 relX = (INT8)((SIZE_T)data - (SIZE_T)((MouseState << 4) & 0x100));
		if ((relX < 0 && MouseX >= (SIZE_T)-relX) || (relX > 0 && MouseX < PS2_MOUSE_MAX_X - (SIZE_T)relX)) MouseX += relX;
		MouseCycle = 2;
	}
	else {
		INT8 relY = (INT8)((SIZE_T)data - (SIZE_T)((MouseState << 3) & 0x100));
		if ((relY < 0 && MouseY >= (SIZE_T)-relY) || (relY > 0 && MouseY < PS2_MOUSE_MAX_Y - (SIZE_T)relY)) MouseY -= relY;
		MouseCycle = 0;
	}
}