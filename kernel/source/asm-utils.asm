bits 32

extern GetCodeSegValue
GetCodeSegValue:
	mov ax, cs
	ret

extern GetDataSegValue
GetDataSegValue:
	mov ax, ds
	ret

extern GetCR0Value
GetCR0Value:
	mov eax, cr0
	ret

extern GetCR2Value
GetCR2Value:
	mov eax, cr2
	ret

extern GetCR4Value
GetCR4Value:
	mov eax, cr4
	ret

extern LoadGDTRegister
LoadGDTRegister:
	mov eax, [esp + 4]
	lgdt [eax]
	ret

extern LoadIDTRegister
LoadIDTRegister:
	mov eax, [esp + 4]
	lidt [eax]
	ret