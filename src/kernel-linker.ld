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

    /* Reserve 8K of memory for GDT32 and aligned at 8K boundary */
    .cpu.gdt ALIGN(8K) : {
        /* TODO: Executable size, load addr, relocation addr optimization */
        _linker_system_gdt = .;
        . += 8*8K;
        ASSERT(. == _linker_system_gdt + 8*8K, "System GDT allocation is not equal with 64K");
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