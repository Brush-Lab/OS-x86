#include <header/system-bus/cpu/cpu.hpp>
#include <header/kernel-linker-symbol.hpp>
#include <header/template/template-memory.hpp>

using namespace KernelLinkerSymbol;
using namespace GlobalDescriptorTable;

GDT32 &CPU::system_gdt = Template::Memory::linker_to_ref_cast<GDT32,&_linker_system_gdt>;

constexpr void CPU::initialize_system_gdt() {
    system_gdt.table[0] = GDT32::SegmentDescriptor();
    system_gdt.table[1] = GDT32::SegmentDescriptor(0xFFFFF, 0, 0, true);
    system_gdt.table[2] = GDT32::SegmentDescriptor(0xFFFFF, 0, 0, true);
    system_gdt.table[3] = GDT32::SegmentDescriptor(0xFFFFF, 0, 3, true);
    system_gdt.table[4] = GDT32::SegmentDescriptor(0xFFFFF, 0, 3, true);
}

CPU::CPU() {
    initialize_system_gdt();
}