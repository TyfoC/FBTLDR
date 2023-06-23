bits 16

extern ResetDiskSystem
ResetDiskSystem:
	push dx
	mov dl, [esp + 6]
	mov ah, 0x00
	int 0x13
	jc .Error
	mov eax, 1
	jmp .Done
.Error:
	mov eax, 0
.Done:
	pop dx
	retd

;	uint8_t ReadSectorsLow(		ESP OFFSET
;		uint8_t diskIndex,			4
;		uint8_t sectorNumber,		8
;		uint8_t headIndex,			12
;		uint16_t cylinderIndex,		16
;		uint8_t count,				20
;		size_t buffSeg,				24
;		size_t buffAddr				28
;	);
extern ReadSectorsLow
ReadSectorsLow:
	push es
	mov ax, [esp + 26]
	mov es, ax

	mov ah, 0x02
	mov al, [esp + 22]

	mov ch, [esp + 18]
	mov cl, [esp + 19]
	shl cl, 6
	or cl, [esp + 10]

	mov dh, [esp + 14]
	mov dl, [esp + 6]

	mov bx, [esp + 30]
	int 0x13
	jc .Error
	mov eax, 1
	jmp .Done
.Error:
	mov eax, 0
.Done:
	pop es
	retd