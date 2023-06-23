COMPILER=i686-elf-gcc
BOCHS=/mnt/c/Installed\ Programs/Bochs-2.7/bochs.exe	# NOT COMMON!
BOCHSDBG=/mnt/c/Installed\ Programs/Bochs-2.7/bochsdbg.exe	# NOT COMMON!
OUT=fobo.bin

COMPILER_BOOTLOADER_FLAGS=-ffreestanding -m16 -std=c17 -Wall -Wextra -O0 -I ../bootloader-main/include
LINKER_BOOTLOADER_FLAGS=-ffreestanding -m16 -nostdlib -O0 -T bootloader-main.ld

COMPILER_KERNEL_FLAGS=-ffreestanding -m32 -std=c17 -Wall -Wextra -O0 -I ../kernel/include
LINKER_KERNEL_FLAGS=-ffreestanding -m32 -nostdlib -O0 -T kernel.ld

C_SRC_BOOTLOADER_MAIN=$(shell find ../bootloader-main/source -name *.c)
C_OBJ_BOOTLOADER_MAIN=$(addsuffix .bootldr.o, $(C_SRC_BOOTLOADER_MAIN))
ASM_SRC_BOOTLOADER_MAIN=$(shell find ../bootloader-main/source -name *.asm)
ASM_OBJ_BOOTLOADER_MAIN=$(addsuffix .bootldr.o, $(ASM_SRC_BOOTLOADER_MAIN))

C_SRC_KERNEL_MAIN=$(shell find ../kernel/source -name *.c)
C_OBJ_KERNEL_MAIN=$(addsuffix .kernel.o, $(C_SRC_KERNEL_MAIN))
ASM_SRC_KERNEL_MAIN=$(shell find ../kernel/source -name *.asm)
ASM_OBJ_KERNEL_MAIN=$(addsuffix .kernel.o, $(ASM_SRC_KERNEL_MAIN))

all:
	$(MAKE) build
	$(MAKE) clear
	$(MAKE) qemu
	rm -f dbg.txt

build:
	$(MAKE) $(C_OBJ_KERNEL_MAIN)
	$(MAKE) $(ASM_OBJ_KERNEL_MAIN)
	$(COMPILER) $(C_OBJ_KERNEL_MAIN) $(ASM_OBJ_KERNEL_MAIN) $(LINKER_KERNEL_FLAGS) -o kernel.bin

	$(MAKE) $(C_OBJ_BOOTLOADER_MAIN)
	$(MAKE) $(ASM_OBJ_BOOTLOADER_MAIN)
	$(COMPILER) $(C_OBJ_BOOTLOADER_MAIN) $(ASM_OBJ_BOOTLOADER_MAIN) $(LINKER_BOOTLOADER_FLAGS) -o bootloader-main.bin
	
	$(MAKE) boot

boot:
	nasm -fbin ../bootloader-setup/setup.asm -o $(OUT)

qemu:
	qemu-system-i386 -monitor stdio -m 2G -cpu max -drive format=raw,file=$(OUT) -D dbg.txt -d cpu_reset

bochs:
	$(BOCHS) -qf win32-bochsrc.bxrc

bochsdbg:
	$(BOCHSDBG) -qf win32-bochsrc.bxrc

clear:
	rm -f bootloader-main.bin kernel.bin $(C_OBJ_BOOTLOADER_MAIN) $(ASM_OBJ_BOOTLOADER_MAIN) $(C_OBJ_KERNEL_MAIN) $(ASM_OBJ_KERNEL_MAIN)

%.c.bootldr.o:	%.c
	$(COMPILER) -c $< $(COMPILER_BOOTLOADER_FLAGS) -o $@

%.asm.bootldr.o:	%.asm
	nasm -felf $< -o $@

%.c.kernel.o:	%.c
	$(COMPILER) -c $< $(COMPILER_KERNEL_FLAGS) -o $@

%.asm.kernel.o:	%.asm
	nasm -felf $< -o $@