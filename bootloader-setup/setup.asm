bits 16
org 0x7c00

%define BOOTLOADER_MAIN_BASE_ADDRESS		0x1000
%define SECTOR_SIZE					512
%define ALIGN_UP(value, alignment)	(((value) + (alignment) - 1) & ~((alignment) - 1))

SetupBegin:
	jmp 0x0000:SetupRegisters
BootloaderSignature:			dd	0x52544246		;	`FBTR` - FBTLDR
BootloaderVersion:				dd	0x00000000
SetupRegisters:
	mov ax, 0
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov bp, BOOTLOADER_MAIN_BASE_ADDRESS
	mov sp, bp
	sti
	cld

	mov al, 0x03
	int 0x10

	mov ax, 0x1003
	mov bx, 0x0000
	int 0x10

	mov ah, 0x01
	mov cx, 0x0706
	int 0x10

	mov ah, 0x00
	int 0x13

	mov ah, 0x02
	mov al, ALIGN_UP(MainEnd - MainBegin, SECTOR_SIZE) / SECTOR_SIZE
	mov cx, 0x0002
	mov dh, 0x00
	mov bx, BOOTLOADER_MAIN_BASE_ADDRESS
	int 0x13
	jc PrintErrorMessage

	push dword ALIGN_UP(MainEnd - SetupBegin, SECTOR_SIZE) / SECTOR_SIZE
	and edx, 0xff
	push edx
	push dword 0
	jmp [BOOTLOADER_MAIN_BASE_ADDRESS]
PrintErrorMessage:
	mov ah, 0x0e
	mov si, ErrorMessage
.Loop:
	lodsb
	test al, al
	jz .Done
	int 0x10
	jmp .Loop
.Done:
	cli
	hlt
	jmp $
ErrorMessage:	db "Failed to load main part of the bootloader!", 0

times SECTOR_SIZE - 2 - $ + $$ db 0
dw 0xaa55

MainBegin:
incbin "bootloader-main.bin"
MainEnd:

KernelBegin:
incbin "kernel.bin"
KernelEnd:

UserConfigBegin:
VideoModeType:			dw 2
VideoModeIndex:			dw 0
VideoModeWidth:			dw 800
VideoModeHeight:		dw 600
VideoModeBitsPerPixel:	dw 24
VideoModeFuzzy:			dw 1
KernelFileType:			dw 0xFFFF