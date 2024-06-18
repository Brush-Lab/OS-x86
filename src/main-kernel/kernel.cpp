#include <stdint.h>
#include <header/system-bus/system-bus.hpp>

class Kernel {
    SystemBus system_bus;
public:
    Kernel() {
    }
};

extern "C" [[gnu::noreturn]] void cdecl_kernel_setup() {
    [[gnu::unused]] Kernel kernel;
    for (int i = 0; i < 80; i++) {
        ((volatile uint16_t*) 0xB8000)[i] = 0x0F00 | 'a'; 
    }
    while(1);
}

