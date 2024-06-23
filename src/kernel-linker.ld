ENTRY(kernel_entrypoint) /* the name of the entry label */

_linker_kernel_entrypoint_stack = 4M - 4;

SECTIONS {
    /* . = 0x00100000; */
    . = 0xC0100000;
    _linker_kernel_virtual_addr_start = .; 
    _linker_kernel_virtual_addr_start = . - 0xC0000000
    .multiboot ALIGN (0x1000): AT (ADDR (.multiboot) - 0xC0000000) { 
        *(.multiboot)  /* put GRUB multiboot header at front */
    }

    .setup.text ALIGN (0x1000): {
        *(.setup.text) /* initial setup code */
    }

    /* all text sections from all files */
    .text ALIGN (0x1000): {
        *(.text)
    }

    /* all read-only data sections from all files */
    .rodata ALIGN (0x1000): {
        *(.rodata*)
    }

    /* all data sections from all files */
    .data ALIGN (0x1000): {
        *(.data)
    }

    /* all COMMON & bss sections from all files */
    .bss ALIGN (0x1000): {
        *(COMMON)
        *(.bss)
        [a-z\/]*kernel-entrypoint.o(.bss)
        _linker_kernel_stack_top = .;
    }

    _linker_kernel_virtual_addr_end = .; 
    _linker_kernel_physical_addr_end = . - 0xC0000000;
}