global kernel_entrypoint               ; The entry symbol for ELF
extern kernel_setup                    ; External C or C++ function
extern _linker_kernel_entrypoint_stack ; External label where $esp is located before calling kernel_setup


; Set the magic number & flag and calculate the checksum (magic number + checksum + flags == 0)
GRUB_MULTIBOOT_MAGIC_NUMBER equ 0x1BADB002
FLAGS                       equ 0x0
CHECKSUM                    equ -GRUB_MULTIBOOT_MAGIC_NUMBER 





; GRUB multiboot header, constants must be 4 bytes aligned
section .multiboot
align 4
    dd GRUB_MULTIBOOT_MAGIC_NUMBER 
    dd FLAGS
    dd CHECKSUM



section .text
; Preventive measure: Disable interrupt. Setup stack register then jump to C or C++ code
; In case kernel_setup return, do Halt and Catch Fire (HCF)
kernel_entrypoint:
    cli
    mov  esp, _linker_kernel_entrypoint_stack
    call kernel_setup
    hlt                                       
