#include <cpu-int-handlers.h>

static INT_HANDLER SoftwareIntHandlers[MAX_IDT_ENTRIES_COUNT] = {};

VOID SoftwareIntHandler0(VOID);
VOID SoftwareIntHandler1(VOID);
VOID SoftwareIntHandler2(VOID);
VOID SoftwareIntHandler3(VOID);
VOID SoftwareIntHandler4(VOID);
VOID SoftwareIntHandler5(VOID);
VOID SoftwareIntHandler6(VOID);
VOID SoftwareIntHandler7(VOID);
VOID SoftwareIntHandler8(VOID);
VOID SoftwareIntHandler9(VOID);
VOID SoftwareIntHandler10(VOID);
VOID SoftwareIntHandler11(VOID);
VOID SoftwareIntHandler12(VOID);
VOID SoftwareIntHandler13(VOID);
VOID SoftwareIntHandler14(VOID);
VOID SoftwareIntHandler15(VOID);
VOID SoftwareIntHandler16(VOID);
VOID SoftwareIntHandler17(VOID);
VOID SoftwareIntHandler18(VOID);
VOID SoftwareIntHandler19(VOID);
VOID SoftwareIntHandler20(VOID);
VOID SoftwareIntHandler21(VOID);
VOID SoftwareIntHandler22(VOID);
VOID SoftwareIntHandler23(VOID);
VOID SoftwareIntHandler24(VOID);
VOID SoftwareIntHandler25(VOID);
VOID SoftwareIntHandler26(VOID);
VOID SoftwareIntHandler27(VOID);
VOID SoftwareIntHandler28(VOID);
VOID SoftwareIntHandler29(VOID);
VOID SoftwareIntHandler30(VOID);
VOID SoftwareIntHandler31(VOID);

VOID InstallSoftwareIntHandler(SIZE_T intIndex, INT_HANDLER intHandler) {
	if (intIndex >= MAX_IDT_ENTRIES_COUNT) return;
	SoftwareIntHandlers[intIndex] = intHandler;
}

VOID UninstallSoftwareIntHandler(SIZE_T intIndex) {
	if (intIndex < MAX_IDT_ENTRIES_COUNT) SoftwareIntHandlers[intIndex] = 0;
}

VOID GetSoftwareIntHandler(SIZE_T intIndex, INT_HANDLER* intHandler) {
	*intHandler = intIndex >= MAX_IDT_ENTRIES_COUNT ? 0 : SoftwareIntHandlers[intIndex];
}

VOID InitSoftwareIntHandlers(IDT_ENTRY* idt, UINT16 codeSegValue) {
	SEGMENT_SELECTOR codeSegSelector;
	InitSegmentSelector(codeSegValue, &codeSegSelector);

	InitIDTEntry((UINT32)SoftwareIntHandler0, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[0]);
	InitIDTEntry((UINT32)SoftwareIntHandler1, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[1]);
	InitIDTEntry((UINT32)SoftwareIntHandler2, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[2]);
	InitIDTEntry((UINT32)SoftwareIntHandler3, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[3]);
	InitIDTEntry((UINT32)SoftwareIntHandler4, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[4]);
	InitIDTEntry((UINT32)SoftwareIntHandler5, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[5]);
	InitIDTEntry((UINT32)SoftwareIntHandler6, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[6]);
	InitIDTEntry((UINT32)SoftwareIntHandler7, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[7]);
	InitIDTEntry((UINT32)SoftwareIntHandler8, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[8]);
	InitIDTEntry((UINT32)SoftwareIntHandler9, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[9]);
	InitIDTEntry((UINT32)SoftwareIntHandler10, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[10]);
	InitIDTEntry((UINT32)SoftwareIntHandler11, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[11]);
	InitIDTEntry((UINT32)SoftwareIntHandler12, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[12]);
	InitIDTEntry((UINT32)SoftwareIntHandler13, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[13]);
	InitIDTEntry((UINT32)SoftwareIntHandler14, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[14]);
	InitIDTEntry((UINT32)SoftwareIntHandler15, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[15]);
	InitIDTEntry((UINT32)SoftwareIntHandler16, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[16]);
	InitIDTEntry((UINT32)SoftwareIntHandler17, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[17]);
	InitIDTEntry((UINT32)SoftwareIntHandler18, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[18]);
	InitIDTEntry((UINT32)SoftwareIntHandler19, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[19]);
	InitIDTEntry((UINT32)SoftwareIntHandler20, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[20]);
	InitIDTEntry((UINT32)SoftwareIntHandler21, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[21]);
	InitIDTEntry((UINT32)SoftwareIntHandler22, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[22]);
	InitIDTEntry((UINT32)SoftwareIntHandler23, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[23]);
	InitIDTEntry((UINT32)SoftwareIntHandler24, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[24]);
	InitIDTEntry((UINT32)SoftwareIntHandler25, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[25]);
	InitIDTEntry((UINT32)SoftwareIntHandler26, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[26]);
	InitIDTEntry((UINT32)SoftwareIntHandler27, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[27]);
	InitIDTEntry((UINT32)SoftwareIntHandler28, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[28]);
	InitIDTEntry((UINT32)SoftwareIntHandler29, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[29]);
	InitIDTEntry((UINT32)SoftwareIntHandler30, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[30]);
	InitIDTEntry((UINT32)SoftwareIntHandler31, codeSegSelector, IDT_GATE_TYPE_INT32, IDT_FLAG_PRESENT, &idt[31]);
}

extern VOID SoftwareInterruptHandler(INT_HANDLER_REGISTERS* registers) {
	if (SoftwareIntHandlers[registers->InterruptIndex]) SoftwareIntHandlers[registers->InterruptIndex](registers);
	else {
		PrintFormatted(
			"Interrupt (Index: 0x%xu, Error code: 0x%xu/0b%bu):\r\n", BIOS_COLOR_YELLOW,
			registers->InterruptIndex, registers->ErrorCode, registers->ErrorCode
		);

		PrintFormatted(
			"\tEFLAGS: 0b%bu, CS: 0x%xu, EIP: 0x%xu\r\n", BIOS_COLOR_LIGHT_BLUE,
			registers->EFLAGS, registers->CS, registers->EIP
		);

		PrintFormatted(
			"\tDS: 0x%xu, ES: 0x%xu, FS: 0x%xu, GS: 0x%xu\r\n", BIOS_COLOR_LIGHT_BLUE,
			registers->DS, registers->ES, registers->FS, registers->GS
		);

		PrintFormatted(
			"\tEAX: 0x%xu, ECX: 0x%xu, EDX: 0x%xu, EBX: 0x%xu, ESI: 0x%xu, EDI: 0x%xu\r\n", BIOS_COLOR_LIGHT_BLUE,
			registers->EAX, registers->ECX, registers->EDX, registers->EBX, registers->ESI, registers->EDI
		);

		PrintFormatted(
			"\tCR0: 0b%bu, CR2: 0b%bu, CR4: 0b%bu\r\n", BIOS_COLOR_LIGHT_BLUE,
			GetCR0Value(), GetCR2Value(), GetCR4Value()
		);
	}
}