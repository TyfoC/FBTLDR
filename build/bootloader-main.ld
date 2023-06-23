ENTRY(BootloaderMain)
OUTPUT_ARCH(i386)
OUTPUT_FORMAT(binary)
SECTIONS {
	__PTR_BOOTLOADER_BASE_ADDRESS__ = 0x1000;
	. = __PTR_BOOTLOADER_BASE_ADDRESS__;

	.header : {
		SHORT(BootloaderMain);
	}

	.text : {
		*(.text)
		. = ALIGN(512);
	}

	.data : {
		*(.data)
		. = ALIGN(512);
	}

	.rodata : {
		*(.rodata)
		. = ALIGN(512);
	}

	.bss : {
		*(.bss)
		. = ALIGN(512);
	}

	__PTR_BOOTLOADER_END_ADDRESS__ = .;
}