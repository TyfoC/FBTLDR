ENTRY(KernelMain)
OUTPUT_ARCH(i386)
OUTPUT_FORMAT(binary)
SECTIONS {
	__PTR_KERNEL_BASE_ADDRESS__ = 0x10000;
	. = __PTR_KERNEL_BASE_ADDRESS__;

	.header : {
		LONG(__PTR_KERNEL_BASE_ADDRESS__);
		LONG(__PTR_KERNEL_END_ADDRESS__ - __PTR_KERNEL_BASE_ADDRESS__);
		LONG(KernelMain);
		LONG(__PTR_STACK_TOP_ADDRESS__);
	}

	.stack : ALIGN(512) {
		. = . + 4K;
		__PTR_STACK_TOP_ADDRESS__ = .;
	}

	.text : ALIGN(512) {
		*(.text)
	}

	.data : ALIGN(512) {
		*(.data)
	}

	.rodata : ALIGN(512) {
		*(.rodata)
	}

	.bss : ALIGN(512) {
		*(.bss)
		. = ALIGN(512);
	}

	__PTR_KERNEL_END_ADDRESS__ = .;
}