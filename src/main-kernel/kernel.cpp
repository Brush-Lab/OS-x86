#include <stdint.h>
#include <header/cpu/gdt.hpp>

class Kernel {
    // static constexpr GlobalDescriptorTable::GDT32 system_gdt;
public:
    Kernel() {
    }
};

extern "C" [[gnu::noreturn]] void kernel_setup() {
    [[gnu::unused]] Kernel kernel;
    for (int i = 0; i < 80; i++) {
        ((volatile uint16_t*) 0xB8000)[i] = 0x0F00 | 'a'; 
    }
    while(1);
}

