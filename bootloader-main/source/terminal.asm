bits 16

extern PutChar
PutChar:
	mov ah, 0x0e
	mov al, [esp + 4]
	int 0x10
	retd