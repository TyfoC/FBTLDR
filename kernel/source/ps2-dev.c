#include <ps2-dev.h>

static BOOL DevsInitialized = FALSE;
static UINT8 FirstDevBuffer[PS2_DEV_BUFFER_LENGTH];
static UINT8 SecondDevBuffer[PS2_DEV_BUFFER_LENGTH];
static SIZE_T FirstDevBufferLength = 0;
static SIZE_T SecondDevBufferLength = 0;
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
	return devIndex == PS2_DEV_FIRST ? FirstDevBufferLength : SecondDevBufferLength;
}

UINT8 PS2DevBufferPopLastByte(SIZE_T devIndex) {
	UINT8 result;
	if (devIndex == PS2_DEV_FIRST) {
		while (!FirstDevBufferLength) {}
		result = FirstDevBuffer[0];
		--FirstDevBufferLength;
		for (SIZE_T i = 0; i < FirstDevBufferLength; i++) FirstDevBuffer[i] = FirstDevBuffer[i + 1];
	}
	else {
		while (!SecondDevBufferLength) {}
		result = SecondDevBuffer[0];
		--SecondDevBufferLength;
		for (SIZE_T i = 0; i < SecondDevBufferLength; i++) SecondDevBuffer[i] = SecondDevBuffer[i + 1];
	}

	return result;
}

UINT8 PS2DevBufferPopFirstByte(SIZE_T devIndex) {
	UINT8 result;
	if (devIndex == PS2_DEV_FIRST) {
		while (!FirstDevBufferLength) {}
		result = FirstDevBuffer[--FirstDevBufferLength];
	}
	else {
		while (!SecondDevBufferLength) {}
		result = SecondDevBuffer[--SecondDevBufferLength];
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
	for (SIZE_T i = FirstDevBufferLength; i > 0; i--) FirstDevBuffer[i] = FirstDevBuffer[i - 1];
	
	FirstDevBuffer[0] = data;
	if (FirstDevBufferLength < PS2_DEV_BUFFER_LENGTH) ++FirstDevBufferLength;
}

VOID PS2SecondHandler(INT_HANDLER_REGISTERS*) {
	UINT8 data = InByte(PS2_CTRL_PORT_DATA);
	for (SIZE_T i = SecondDevBufferLength; i > 0; i--) SecondDevBuffer[i] = SecondDevBuffer[i - 1];
	
	SecondDevBuffer[0] = data;
	if (SecondDevBufferLength < PS2_DEV_BUFFER_LENGTH) ++SecondDevBufferLength;
}