bits 16

extern GetMemoryMapElement
GetMemoryMapElement:
	push es
	mov eax, 0x0000e820
	mov ecx, 0x00000018
	mov edx, 0x534d4150
	mov edi, [esp + 6]			;	buffer segment
	mov es, di
	mov edi, [esp + 10]			;	buffer offset
	mov ebx, [esp + 14]			;	previous BIOS memory map offset
	mov dword [es:di + 20], 1
	int 0x15
	jc .Error
	cmp eax, 0x534d4150
	jne .Error
	mov eax, ebx
	jmp .Done
.Error:
	mov eax, 0
.Done:
	pop es
	retd