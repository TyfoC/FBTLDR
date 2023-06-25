bits 32

extern OutByte
OutByte:
	mov dx, [esp + 4]
	mov al, [esp + 8]
	out dx, al
	ret

extern OutWord
OutWord:
	mov dx, [esp + 4]
	mov ax, [esp + 8]
	out dx, ax
	ret

extern OutLong
OutLong:
	mov dx, [esp + 4]
	mov eax, [esp + 8]
	out dx, eax
	ret

extern InByte
InByte:
	mov dx, [esp + 4]
	in al, dx
	ret

extern InWord
InWord:
	mov dx, [esp + 4]
	in ax, dx
	ret

extern InLong
InLong:
	mov dx, [esp + 4]
	in eax, dx
	ret