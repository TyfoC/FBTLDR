bits 32

extern LoadGDTRegister
LoadGDTRegister:
	mov eax, [esp + 4]
	cli
	lgdt [eax]
	ret