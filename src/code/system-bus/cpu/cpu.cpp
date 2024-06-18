#include <header/system-bus/cpu/cpu.hpp>
#include <header/kernel-linker-symbol.hpp>

using namespace GlobalDescriptorTable;
using namespace KernelLinkerSymbol;

// C++ idiom: "Construct on first use", to avoid "Static initialization order fiasco"
GDT32 &CPU::get_system_gdt() {
    static GDT32 *system_gdt = reinterpret_cast<GDT32*>(&_linker_system_gdt); 
    // Extra: Optimization, remove guard variable & check for this static var system_gdt 
    return *system_gdt;
}

void CPU::initialize_system_gdt() {
    gdt.table[0] = GDT32::SegmentDescriptor();
    gdt.table[1] = GDT32::SegmentDescriptor(0xFFFFF, 0, 0, true);
    gdt.table[2] = GDT32::SegmentDescriptor(0xFFFFF, 0, 0, true);
    gdt.table[3] = GDT32::SegmentDescriptor(0xFFFFF, 0, 3, true);
    gdt.table[4] = GDT32::SegmentDescriptor(0xFFFFF, 0, 3, true);
}

CPU::CPU() : gdt(get_system_gdt()) {
    initialize_system_gdt();
}