#ifndef _CPU_HPP
#define _CPU_HPP

#include <header/system-bus/cpu/gdt.hpp>
#include <header/kernel-linker-symbol.hpp>

class CPU {
    static GlobalDescriptorTable::GDT32 &get_system_gdt();
    GlobalDescriptorTable::GDT32 &gdt;

    void initialize_system_gdt();
public:
    CPU();
};

#endif