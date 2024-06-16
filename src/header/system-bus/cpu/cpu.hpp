#ifndef _CPU_HPP
#define _CPU_HPP

#include <header/system-bus/cpu/gdt.hpp>
#include <header/kernel-linker-symbol.hpp>

class CPU {
    static struct GlobalDescriptorTable::GDT32 &system_gdt;

    constexpr void initialize_system_gdt();

public:
    CPU();
};

#endif