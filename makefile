# Compiler & linker
ASM           = nasm
LIN           = ld
CPPC          = g++

# Directory
SOURCE_FOLDER = src
OUTPUT_FOLDER = bin
OUTPUT_KERNEL = bin/kernel
ISO_NAME      = OS86
DISK_NAME     = storage

# Flags
WARNING_CFLAG   = -Wall -Wextra -Werror
DEBUG_CFLAG     = -fshort-wchar -g
STRIP_CFLAG     = -nostdlib -fno-stack-protector -nostartfiles -nodefaultlibs -ffreestanding
REGISTER_CFLAG  = -mgeneral-regs-only -mno-sse -mno-mmx
OPTIMIZER_CFLAG = -O1
CFLAGS          = $(DEBUG_CFLAG) $(WARNING_CFLAG) $(STRIP_CFLAG) $(REGISTER_CFLAG) $(OPTIMIZER_CFLAG) -m32 --std=c++20 -c -I$(SOURCE_FOLDER)
AFLAGS          = -f elf32 -g -F dwarf
LFLAGS          = -T $(SOURCE_FOLDER)/kernel-linker.ld -m elf_i386 

#recursive wildcard
# so basically this shit below is wildcard but recursive
# $(wildcard $1*): ini untuk menemukan semua file di direktori jadi kayak misal sekarang di folder src maka bakalan nyari file .cpp di src contoh: src/bro.cpp src/akugila.cpp
# $(foreach d,...): buat ngulangin setiap direktori yang ketemu sebelumnya \
	d disini merepresentasikan semua item kayak direktori-nya
# $(call rwildcard,$d/,$2): $d merepresentasikan directory-directory sebelumnya \
	jika ada src/ seperti src/code src/asm maka bagian ini akan mencari secara rekursif direktori code / asm
# $(filter $(subst *,%,$2),$d): Fungsi filter digunakan untuk menyaring elemen dalam daftar berdasarkan pola tertentu. Di sini, $(subst *,%,$2) menggantikan setiap * dalam pola dengan % \
  Contoh: jika $2 adalah .cpp maka ini akan menyaring file yang berjenis .cpp saja

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
	rm -rf bin/
	rm -rf $(OUTPUT_FOLDER)/*.iso
	rm -rf $(OUTPUT_FOLDER)/kernel

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