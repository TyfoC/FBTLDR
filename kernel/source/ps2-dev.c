#include <ps2-dev.h>

static BOOL DevsInitialized = FALSE;
UINT8 FirstPS2DevBuffer[PS2_DEV_BUFFER_LENGTH];
UINT8 SecondPS2DevBuffer[PS2_DEV_BUFFER_LENGTH];
SIZE_T FirstPS2DevBufferLength = 0;
SIZE_T SecondPS2DevBufferLength = 0;
static PS2_DEV_INT_HANDLER FirstPS2DevIRQHandler;
static PS2_DEV_INT_HANDLER SecondPS2DevIRQHandler;
static SIZE_T KbdDevIndex = NPOS;
static SIZE_T MouseDevIndex = NPOS;
static UINT16 KbdDevType = PS2_DEV_TYPE_UNDEFINED;
static UINT16 MouseDevType = PS2_DEV_TYPE_UNDEFINED;

extern VOID PS2FirstHandler(INT_HANDLER_REGISTERS*);
extern VOID PS2SecondHandler(INT_HANDLER_REGISTERS*);

BOOL PS2DevsInit(VOID) {
	BOOL firstDevAvailable = PS2CtrlFirstPortAvailable();
	BOOL secondDevAvailable = PS2CtrlSecondPortAvailable();

	PS2CtrlSendCommand(PS2_CTRL_COMMAND_READ_CONFIG);
	UINT8 config = PS2CtrlReadAnswer();
	if (firstDevAvailable) {
		InstallHardwareIntHandler(PS2_DEV_FIRST_IRQ, PS2FirstHandler);
		UnmaskPICIRQ(PS2_DEV_FIRST_IRQ);
		config |= PS2_CTRL_CONFIG_FIRST_PORT_INT_ENABLED;

		if (PS2DevSendByte(PS2_DEV_FIRST, PS2_DEV_COMMAND_ENABLE_DATA_REPORTING, TRUE) != PS2_DEV_ANSWER_ACK) return FALSE;
	}

	if (secondDevAvailable) {
		InstallHardwareIntHandler(PS2_DEV_SECOND_IRQ, PS2SecondHandler);
		UnmaskPICIRQ(PS2_DEV_SECOND_IRQ);
		config |= PS2_CTRL_CONFIG_SECOND_PORT_INT_ENABLED;
		
		if (PS2DevSendByte(PS2_DEV_SECOND, PS2_DEV_COMMAND_ENABLE_DATA_REPORTING, TRUE) != PS2_DEV_ANSWER_ACK) return FALSE;
	}

	PS2CtrlSendCommand(PS2_CTRL_COMMAND_WRITE_CONFIG);
	PS2CtrlSendData(config);
	PS2CtrlFlushBuffer();

	UINT16 devType;
	if (firstDevAvailable) {
		if (!PS2DevReset(PS2_DEV_FIRST)) return FALSE;

		devType = PS2DevGetType(PS2_DEV_FIRST);
		if (
			devType == PS2_DEV_TYPE_ANCIENT_AT_KBD ||
			(devType & 0xFF00) == 0xAB00 ||
			(devType & 0xFF00) == 0xAC00
		) {
			KbdDevIndex = PS2_DEV_FIRST;
			KbdDevType = devType;
		}
		else {
			MouseDevIndex = PS2_DEV_FIRST;
			MouseDevType = devType;
		}
	}

	if (secondDevAvailable) {
		if (!PS2DevReset(PS2_DEV_SECOND)) return FALSE;

		devType = PS2DevGetType(PS2_DEV_SECOND);
		if (
			devType == PS2_DEV_TYPE_ANCIENT_AT_KBD ||
			(devType & 0xFF00) == 0xAB00 ||
			(devType & 0xFF00) == 0xAC00
		) {
			KbdDevIndex = PS2_DEV_SECOND;
			KbdDevType = devType;
		}
		else {
			MouseDevIndex = PS2_DEV_SECOND;
			MouseDevType = devType;
		}
	}

	DevsInitialized = TRUE;
	return TRUE;
}

BOOL PS2DevsInitialized(VOID) {
	return DevsInitialized;
}

/*
	devIndex:
		0 - first
		1 - second
	ctrlData:
		0 - read answer from PS/2 dev buffer (IRQ)
		1 - read answer from PS/2 controller data port
*/
UINT8 PS2DevSendByte(SIZE_T devIndex, UINT8 value, BOOL ctrlData) {
	if (devIndex == PS2_DEV_FIRST) {
		if (!PS2CtrlFirstPortAvailable()) return FALSE;
	}
	else {
		if (!PS2CtrlSecondPortAvailable()) return FALSE;
		PS2CtrlSendCommand(PS2_CTRL_COMMAND_WRITE_SECOND_PORT);
	}

	UINT8 answer;
	SIZE_T curNumOfAttempts = PS2_DEV_NUMBER_OF_ATTEMPTS;
	do {
		PS2CtrlSendData(value);
		Sleep(PS2_DEV_RESPONSE_TIMEOUT);

		answer = ctrlData ? PS2CtrlReadAnswer() : PS2DevBufferPopFirstByte(devIndex);
		if (answer == PS2_DEV_ANSWER_RESEND) {
			if (curNumOfAttempts) --curNumOfAttempts;
			else break;
		}
	} while (answer == PS2_DEV_ANSWER_RESEND);

	return answer;
}


BOOL PS2DevBufferFull(SIZE_T devIndex) {
	return devIndex == PS2_DEV_FIRST ? FirstPS2DevBufferLength : SecondPS2DevBufferLength;
}

UINT8 PS2DevBufferPopLastByte(SIZE_T devIndex) {
	UINT8 result;
	if (devIndex == PS2_DEV_FIRST) {
		while (!FirstPS2DevBufferLength) {}
		result = FirstPS2DevBuffer[0];
		--FirstPS2DevBufferLength;
		for (SIZE_T i = 0; i < FirstPS2DevBufferLength; i++) FirstPS2DevBuffer[i] = FirstPS2DevBuffer[i + 1];
	}
	else {
		while (!SecondPS2DevBufferLength) {}
		result = SecondPS2DevBuffer[0];
		--SecondPS2DevBufferLength;
		for (SIZE_T i = 0; i < SecondPS2DevBufferLength; i++) SecondPS2DevBuffer[i] = SecondPS2DevBuffer[i + 1];
	}

	return result;
}

UINT8 PS2DevBufferPopFirstByte(SIZE_T devIndex) {
	UINT8 result;
	if (devIndex == PS2_DEV_FIRST) {
		while (!FirstPS2DevBufferLength) {}
		result = FirstPS2DevBuffer[--FirstPS2DevBufferLength];
	}
	else {
		while (!SecondPS2DevBufferLength) {}
		result = SecondPS2DevBuffer[--SecondPS2DevBufferLength];
	}

	return result;
}

VOID PS2DevEmptyBuffer(SIZE_T devIndex) {
	while (PS2DevBufferFull(devIndex)) PS2DevBufferPopLastByte(devIndex);
}

BOOL PS2DevReset(SIZE_T devIndex) {
	UINT8 answer = PS2DevSendByte(devIndex, PS2_DEV_COMMAND_RESET, FALSE);
	if (answer != PS2_DEV_ANSWER_ACK) return FALSE;

	answer = PS2DevBufferPopFirstByte(devIndex);
	if (answer != PS2_DEV_ANSWER_RESET_DONE) return FALSE;
	
	PS2DevEmptyBuffer(devIndex);														//	PS/2 mouse can send ID byte (0x00)
	answer = PS2DevSendByte(devIndex, PS2_DEV_COMMAND_ENABLE_DATA_REPORTING, FALSE);	//	PS/2 mouse stops data reporting after reset command
	return answer == PS2_DEV_ANSWER_ACK;
}

VOID PS2DevSetIRQHandler(SIZE_T devIndex, PS2_DEV_INT_HANDLER intHandler) {
	if (devIndex == PS2_DEV_FIRST) FirstPS2DevIRQHandler = intHandler;
	else SecondPS2DevIRQHandler = intHandler;
}

UINT16 PS2DevGetType(SIZE_T devIndex) {
	UINT16 devType = 0;

	devType = PS2DevSendByte(devIndex, PS2_DEV_COMMAND_IDENTIFY, FALSE);
	if (devType != PS2_DEV_ANSWER_ACK) return PS2_DEV_TYPE_UNDEFINED;

	devType = PS2DevBufferPopFirstByte(devIndex);
	if (devType == PS2_CTRL_STATUS_WAIT_FAILED) return PS2_DEV_TYPE_ANCIENT_AT_KBD;
	else if (devType == 0xAC) {
		devType <<= 8;
		devType |= PS2DevBufferPopFirstByte(devIndex);
		return PS2_DEV_TYPE_NCD_SUN_LAYOUT_KBD;
	}

	if (devType == 0xAB) {
		devType <<= 8;
		devType |= PS2DevBufferPopFirstByte(devIndex);
		if (devType == 0xABC1) devType = 0xAB83;
	}

	return devType;
}

UINT16 PS2DevGetKbdType(VOID) {
	return KbdDevType;
}

UINT16 PS2DevGetMouseType(VOID) {
	return MouseDevType;
}

SIZE_T PS2DevGetKbdIndex(VOID) {
	return KbdDevIndex;
}

SIZE_T PS2DevGetMouseIndex(VOID) {
	return MouseDevIndex;
}

VOID PS2FirstHandler(INT_HANDLER_REGISTERS*) {
	UINT8 data = InByte(PS2_CTRL_PORT_DATA);
	SIZE_T count = FirstPS2DevBufferLength < PS2_DEV_BUFFER_LENGTH ? FirstPS2DevBufferLength : PS2_DEV_BUFFER_LENGTH - 1;
	for (SIZE_T i = count; i > 0; i--) FirstPS2DevBuffer[i] = FirstPS2DevBuffer[i - 1];
	
	FirstPS2DevBuffer[0] = data;
	if (FirstPS2DevBufferLength < PS2_DEV_BUFFER_LENGTH) ++FirstPS2DevBufferLength;
	if (FirstPS2DevIRQHandler) FirstPS2DevIRQHandler(data);
}

VOID PS2SecondHandler(INT_HANDLER_REGISTERS*) {
	UINT8 data = InByte(PS2_CTRL_PORT_DATA);
	SIZE_T count = SecondPS2DevBufferLength < PS2_DEV_BUFFER_LENGTH ? SecondPS2DevBufferLength : PS2_DEV_BUFFER_LENGTH - 1;
	for (SIZE_T i = count; i > 0; i--) SecondPS2DevBuffer[i] = SecondPS2DevBuffer[i - 1];
	
	SecondPS2DevBuffer[0] = data;
	if (SecondPS2DevBufferLength < PS2_DEV_BUFFER_LENGTH) ++SecondPS2DevBufferLength;
	if (SecondPS2DevIRQHandler) SecondPS2DevIRQHandler(data);
}