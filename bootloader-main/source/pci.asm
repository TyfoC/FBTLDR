bits 16

extern GetPciSupport
GetPciSupport:
	push ecx
	push edx
	push ebx
	push edi
	mov ax, 0xb101
	mov edi, 0
	int 0x1a
	jc .CheckFailed
	test ah, ah
	jc .CheckFailed
	or al, 0x04			;		PCI_SUPPORT_CHECK_COMPLETED_SUCCESSFULLY
	jmp .Done
.CheckFailed:
	mov al, 0
.Done:
	pop edi
	pop ebx
	pop edx
	pop ecx
	retd