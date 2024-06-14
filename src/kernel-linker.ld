ENTRY(kernel_entrypoint) /* the name of the entry label */

_linker_kernel_entrypoint_stack = 4M - 4;

SECTIONS {
    . = 0x00100000;
    .multiboot : {
        *(.multiboot)  /* put GRUB multiboot header at front */
    }

    .setup.text : {
        *(.setup.text) /* initial setup code */
    }

    /* all text sections from all files */
    .text : {
        *(.text)
    }

    /* all read-only data sections from all files */
    .rodata : {
        *(.rodata*)
    }

    /* all data sections from all files */
    .data : {
        *(.data)
    }

    /* all COMMON & bss sections from all files */
    .bss : {
        *(COMMON)
        *(.bss)
    }
}
