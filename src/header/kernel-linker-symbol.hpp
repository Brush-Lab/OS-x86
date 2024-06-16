#ifndef _KERNEL_LINKER_SYMBOL
#define _KERNEL_LINKER_SYMBOL

#include <stdint.h>

namespace KernelLinkerSymbol {
    // TODO: Nominal types?
    extern "C" {
        uint32_t _linker_system_gdt;
    };
}

#endif