#OBJECTS       = src/kernel.o src/kernel-entrypoint.o src/cpu/gdt.o

# Compiler & linker
ASM           = nasm
LIN           = ld
CPPC          = g++

# Directory
SOURCE_FOLDER = src
# SOURCE_FOLDER_CODE = $(SOURCE_FOLDER)/code
# SOURCE_FOLDER_ASM = $(SOURCE_FOLDER)/asm
# SOURCE_FOLDER_KERNEL = $(SOURCE_FOLDER)/kernel
OUTPUT_FOLDER = bin
OUTPUT_KERNEL = bin/kernel
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

#recursive wildcard
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

#sources file 
CPP_SOURCES := $(call rwildcard,$(SOURCE_FOLDER),*.cpp)
ASM_SOURCES := $(call rwildcard,$(SOURCE_FOLDER),*.s)

#.o files
CPP_OBJECTS := $(patsubst $(SOURCE_FOLDER)/%.cpp,$(OUTPUT_FOLDER)/%.o,$(CPP_SOURCES))
ASM_OBJECTS := $(patsubst $(SOURCE_FOLDER)/%.s,$(OUTPUT_FOLDER)/%.o,$(ASM_SOURCES))

OBJECTS := $(CPP_OBJECTS) $(ASM_OBJECTS) 

run: all
	@qemu-system-i386 -s -S -cdrom $(OUTPUT_FOLDER)/$(ISO_NAME).iso
all: build
build: iso
clean:
	rm -rf *.o $(OUTPUT_FOLDER)/*.iso $(OUTPUT_FOLDER)/kernel ./**/*.o

kernel: $(OBJECTS) #TODO: I have to update $(OBJECTS)
	@echo Linking object files and generate elf32...
	@$(LIN) $(LFLAGS) $^ -o $(OUTPUT_FOLDER)/kernel
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

$(OUTPUT_FOLDER)/%.o: $(SOURCE_FOLDER)/%.cpp
	@mkdir -p $(@D)
	@$(CPPC) $(CFLAGS) $< -o $@
	@echo Compiling $@

$(OUTPUT_FOLDER)/%.o: $(SOURCE_FOLDER)/%.s
	@mkdir -p $(@D)
	@$(ASM) $(AFLAGS) $< -o $@
	@echo Compiling $@...