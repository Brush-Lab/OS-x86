ENTRY(kernel_entrypoint) /* the name of the entry label */

_linker_kernel_entrypoint_stack = 4M - 4;

SECTIONS {
    . = 0x00100000;
    .multiboot : {
        *(.multiboot)        /* put GRUB multiboot header at front */
    }

    .setup.text : {
        *(.setup.text)       /* initial setup code */
    }

    .text : {
        *(.text)             /* all text sections from all files */
    }

    .rodata : {
        *(.rodata*)          /* all read-only data sections from all files */
    }

    .data : {
        *(.data)             /* all data sections from all files */
    }

    .bss : {
        *(COMMON)            /* all COMMON sections from all files */
        *(.bss)              /* all bss sections from all files */
        [a-z\/]*kernel-entrypoint.o(.bss)
    }
}
