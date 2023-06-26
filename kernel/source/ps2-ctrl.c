#include <ps2-ctrl.h>

static BOOL FirstPortAvailable = FALSE;
static BOOL SecondPortAvailable = FALSE;

BOOL InstallPS2Ctrl(VOID) {
	if (ACPIInitialized()) {
		UINT8 majorVer = ACPIGetMajorVersion();
		UINT16 bootArchFlags = GetBootArchitectureFlags();
		if (majorVer >= 2 && !(bootArchFlags & BOOT_ARCH_FLAG_8042)) return FALSE;
	}

	PS2CtrlLockInput();
	PS2CtrlFlushBuffer();

	PS2CtrlSendCommand(PS2_CTRL_COMMAND_READ_CONFIG);
	UINT8 config = PS2CtrlReadAnswer();
	config &= (UINT8)~(
		PS2_CTRL_CONFIG_FIRST_PORT_INT_ENABLED |
		PS2_CTRL_CONFIG_SECOND_PORT_INT_ENABLED |
		PS2_CTRL_CONFIG_FIRST_PORT_CLOCK_DISABLED |
		PS2_CTRL_CONFIG_SECOND_PORT_CLOCK_DISABLED |
		PS2_CTRL_CONFIG_FIRST_PORT_TRANSLATION
	);

	PS2CtrlSendCommand(PS2_CTRL_COMMAND_WRITE_CONFIG);
	PS2CtrlSendData(config);

	PS2CtrlSendCommand(PS2_CTRL_COMMAND_TEST_CTRL);
	UINT8 ctrlTestRes = PS2CtrlReadAnswer();
	PS2CtrlSendCommand(PS2_CTRL_COMMAND_WRITE_CONFIG);
	PS2CtrlSendData(config);

	if (ctrlTestRes != PS2_CTRL_RESPONSE_TEST_PASSED) return FALSE;

	PS2CtrlSendCommand(PS2_CTRL_COMMAND_ENABLE_SECOND_PORT);
	PS2CtrlSendCommand(PS2_CTRL_COMMAND_READ_CONFIG);
	config = PS2CtrlReadAnswer();
	BOOL singleChnl = config & PS2_CTRL_CONFIG_SECOND_PORT_CLOCK_DISABLED;
	if (!singleChnl) PS2CtrlSendCommand(PS2_CTRL_COMMAND_DISABLE_SECOND_PORT);

	PS2CtrlSendCommand(PS2_CTRL_COMMAND_TEST_FIRST_PORT);
	FirstPortAvailable = PS2CtrlReadAnswer() == PS2_CTRL_RESPONSE_PORT_TEST_PASSED;
	if (!singleChnl) {
		PS2CtrlSendCommand(PS2_CTRL_COMMAND_TEST_SECOND_PORT);
		SecondPortAvailable = PS2CtrlReadAnswer() == PS2_CTRL_RESPONSE_PORT_TEST_PASSED;
	}

	PS2CtrlUnlockInput();
	return TRUE;
}

VOID PS2CtrlSendCommand(UINT8 value) {
	OutByte(PS2_CTRL_PORT_COMMAND, value);
}

VOID PS2CtrlSendData(UINT8 value) {
	PS2CtrlWaitUntilReadyToSend();
	OutByte(PS2_CTRL_PORT_DATA, value);
}

BOOL PS2CtrlSendDataWithTimeout(UINT8 value) {
	if (!PS2CtrlWaitUntilReadyToSendWithTimeOut()) return FALSE;
	OutByte(PS2_CTRL_PORT_DATA, value);
	return TRUE;
}

UINT8 PS2CtrlReadAnswer() {
	PS2CtrlWaitUntilReadyToReceive();
	return InByte(PS2_CTRL_PORT_DATA);
}

/*
	Can return PS2_CTRL_STATUS_WAIT_FAILED if failed
*/
UINT8 PS2CtrlReadAnswerWithTimeOut() {
	if (!PS2CtrlWaitUntilReadyToReceiveWithTimeOut()) return PS2_CTRL_STATUS_WAIT_FAILED;
	return InByte(PS2_CTRL_PORT_DATA);
}

BOOL PS2CtrlFirstPortAvailable(VOID) {
	return FirstPortAvailable;
}

BOOL PS2CtrlSecondPortAvailable(VOID) {
	return SecondPortAvailable;
}

BOOL PS2CtrlReadyToSend(VOID) {
	return !(InByte(PS2_CTRL_PORT_STATUS) & PS2_CTRL_STATUS_INPUT_FULL);
}

BOOL PS2CtrlReadyToReceive(VOID) {
	return InByte(PS2_CTRL_PORT_STATUS) & PS2_CTRL_STATUS_OUTPUT_FULL;
}

VOID PS2CtrlWaitUntilReadyToSend(VOID) {
	while (!PS2CtrlReadyToSend()) {}
}

BOOL PS2CtrlWaitUntilReadyToSendWithTimeOut(VOID) {
	SIZE_T curNumberOfAttempts = PS2_CTRL_NUMBER_OF_ATTEMPTS;
	do {
		Sleep(10);
		if (curNumberOfAttempts) --curNumberOfAttempts;
		else return FALSE;
	} while (!PS2CtrlReadyToSend());

	return TRUE;
}

VOID PS2CtrlWaitUntilReadyToReceive(VOID) {
	while (!PS2CtrlReadyToReceive()) {}
}

BOOL PS2CtrlWaitUntilReadyToReceiveWithTimeOut(VOID) {
	SIZE_T curNumberOfAttempts = PS2_CTRL_NUMBER_OF_ATTEMPTS;
	do {
		Sleep(10);
		if (curNumberOfAttempts) --curNumberOfAttempts;
		else return FALSE;
	} while (!PS2CtrlReadyToReceive());

	return TRUE;
}

VOID PS2CtrlLockInput(VOID) {
	PS2CtrlSendCommand(PS2_CTRL_COMMAND_DISABLE_FIRST_PORT);
	PS2CtrlSendCommand(PS2_CTRL_COMMAND_DISABLE_SECOND_PORT);
}

VOID PS2CtrlUnlockInput(VOID) {
	PS2CtrlSendCommand(PS2_CTRL_COMMAND_ENABLE_FIRST_PORT);
	PS2CtrlSendCommand(PS2_CTRL_COMMAND_ENABLE_SECOND_PORT);
}

VOID PS2CtrlFlushBuffer(VOID) {
	while (PS2CtrlReadyToReceive()) PS2CtrlReadAnswer();
}

VOID PS2CtrlReboot(VOID) {
	DISABLE_HARDWARE_INTERRUPTS();
	PS2CtrlFlushBuffer();
	PS2CtrlWaitUntilReadyToSend();
	PS2CtrlSendCommand(PS2_CTRL_COMMAND_PULSE_OUTPUT_LINE);
}