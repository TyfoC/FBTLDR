bits 16

extern SwitchToProtectedMode
SwitchToProtectedMode:
	mov eax, [esp + 4]		;	gdt reg address
	mov ecx, [esp + 8]		;	ret address
	mov dx, [esp + 12]		;	code segment
	mov bx, [esp + 16]		;	data segment
	mov ebp, [esp + 20]		;	stack top address
	sub ebp, 4				;	prevent data corruption
	cli
	lgdt [eax]
	mov eax, cr0
	or al, 1
	mov cr0, eax
	push dx					;	far jump
	push .InitRegisters
	retfw
.InitRegisters:
bits 32
	mov ds, bx
	mov es, bx
	mov fs, bx
	mov gs, bx
	mov ss, bx
	mov esp, ebp
	jmp ecx
bits 16