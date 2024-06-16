#include <header/system-bus/cpu/cpu.hpp>
#include <header/kernel-linker-symbol.hpp>

using namespace KernelLinkerSymbol;

GlobalDescriptorTable::GDT32 &CPU::system_gdt = *(static_cast<GlobalDescriptorTable::GDT32*>(static_cast<void*>(&_linker_system_gdt)));

constexpr void CPU::initialize_system_gdt() {
    system_gdt.table[0] = GlobalDescriptorTable::GDT32::SegmentDescriptor();
    system_gdt.table[1] = GlobalDescriptorTable::GDT32::SegmentDescriptor(0xFFFFF, 0, 0, true);
    system_gdt.table[2] = GlobalDescriptorTable::GDT32::SegmentDescriptor(0xFFFFF, 0, 0, true);
    system_gdt.table[3] = GlobalDescriptorTable::GDT32::SegmentDescriptor(0xFFFFF, 0, 3, true);
    system_gdt.table[4] = GlobalDescriptorTable::GDT32::SegmentDescriptor(0xFFFFF, 0, 3, true);
}

CPU::CPU() {
    initialize_system_gdt();
}