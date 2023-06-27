# FBTLDR - Bootloader for x86 architecture
###	TODO List:
-	[x] Primary bootloader (written in ASM)
	+	[x] Loading a secondary bootloader
-	[x] Secondary bootloader (written in C)
	+	[x] Loading the kernel into memory
	+	[x] Initializing GDT
	+	[x] Getting memory map
	+	[x] Getting info about PCI
	+	[x] Jumping to kernel
-	[ ] Kernel
	+	[x] [TTY](kernel/source/terminal.c)
	+	[x] [GDT](kernel/source/cpu-desc-tables.c)
	+	[x] [IDT](kernel/source/cpu-desc-tables.c)
	+	[x] [PIC](kernel/source/pic.c)
	+	[x] [ISRs](kernel/source/cpu-int-handlers.c)
	+	[x] [IRQs](kernel/source/cpu-int-handlers.c)
	+	[x] [PMM](kernel/source/memory.c)
	+	[x] [PIT](kernel/source/pit.c)
	+	[ ] RTC
	+	[ ] HPET
	+	[x] [PCI](kernel/source/pci.c)
	+	[x] PS/2
		*	[x] [PS/2 Controller](kernel/source/ps2-ctrl.c)
		*	[x] [PS/2 Keyboard](kernel/source/ps2-kbd.c)
		*	[x] [PS/2 Mouse](kernel/source/ps2-mouse.c)
	+	[ ] ATA
	+	[ ] GUI Driver
	+	[ ] Other stuff