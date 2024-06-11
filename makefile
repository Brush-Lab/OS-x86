OBJECTS       = src/kernel.o src/kernel-entrypoint.o src/cpu/gdt.o

# Compiler & linker
ASM           = nasm
LIN           = ld
CPPC          = g++

# Directory
SOURCE_FOLDER = src
OUTPUT_FOLDER = bin
ISO_NAME      = OS86
DISK_NAME     = storage

# Flags
WARNING_CFLAG  = -Wall -Wextra -Werror
DEBUG_CFLAG    = -fshort-wchar -g
STRIP_CFLAG    = -nostdlib -fno-stack-protector -nostartfiles -nodefaultlibs -ffreestanding
REGISTER_CFLAG = -mgeneral-regs-only -mno-sse -mno-mmx
CFLAGS         = $(DEBUG_CFLAG) $(WARNING_CFLAG) $(STRIP_CFLAG) $(REGISTER_CFLAG) -m32 --std=c++20 -c -I$(SOURCE_FOLDER)
AFLAGS         = -f elf32 -g -F dwarf
LFLAGS         = -T $(SOURCE_FOLDER)/kernel-linker.ld -m elf_i386


run: all
	@qemu-system-i386 -s -S -cdrom $(OUTPUT_FOLDER)/$(ISO_NAME).iso
all: build
build: iso
clean:
	rm -rf *.o $(OUTPUT_FOLDER)/*.iso $(OUTPUT_FOLDER)/kernel ./**/*.o

kernel: $(OBJECTS)
	@echo Linking object files and generate elf32...
	@$(LIN) $(LFLAGS) $(OBJECTS) -o $(OUTPUT_FOLDER)/kernel
	@rm -f *.o

iso: kernel
	@mkdir -p $(OUTPUT_FOLDER)/iso/boot/grub
	@cp $(OUTPUT_FOLDER)/kernel     $(OUTPUT_FOLDER)/iso/boot/
	@cp other/grub1                 $(OUTPUT_FOLDER)/iso/boot/grub/
	@cp $(SOURCE_FOLDER)/menu.lst   $(OUTPUT_FOLDER)/iso/boot/grub/
	@genisoimage -R                   \
		-b boot/grub/grub1         \
		-no-emul-boot              \
		-boot-load-size 4          \
		-A os                      \
		-input-charset utf8        \
		-quiet                     \
		-boot-info-table           \
		-o $(OUTPUT_FOLDER)/$(ISO_NAME).iso \
		$(OUTPUT_FOLDER)/iso
	@rm -r $(OUTPUT_FOLDER)/iso/

%.o: %.cpp
	@mkdir -p $(@D)
	@$(CPPC) $(CFLAGS) $< -o $@
	@echo Compiling $@...

%.o: %.s
	@mkdir -p $(@D)
	@$(ASM) $(AFLAGS) $< -o $@
	@echo Compiling $@...