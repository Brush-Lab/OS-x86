#ifndef _GDT_HPP
#define _GDT_HPP

#include <stdint.h>

namespace GlobalDescriptorTable {
    constexpr uint32_t GDT32_ENTRY_COUNT = 1024;

    struct [[gnu::packed]] GDT32 {
        struct [[gnu::packed]] SegmentDescriptor {
            uint16_t segment_low;
            uint16_t base_low;

            // Next 16-bit (Bit 32 to 47)
            uint8_t base_mid;
            union AccessByte {
                struct [[gnu::packed]] {
                    uint8_t type_bit   : 4;
                    uint8_t non_system : 1;
                    uint8_t privilege  : 2;
                    uint8_t valid_bit  : 1;
                } bitfield;
                uint8_t byte;
            } access;

            // Next 8-bit (Bit 48 to 55)
            uint8_t segment_high : 4;
            uint8_t _reserved    : 1;
            uint8_t long_mode    : 1;
            uint8_t opr_32_bit   : 1;
            uint8_t granularity  : 1;

            // Last 8-bit (Bit 56 to 63)
            uint8_t base_high;

            constexpr void set_segment_address(uint32_t segment);
            constexpr void set_base_address(uint32_t base);
            constexpr SegmentDescriptor();
            constexpr SegmentDescriptor(
                uint32_t segment,
                uint32_t base,
                uint8_t  privilege,
                bool     code_descriptor
            );
        } table[GDT32_ENTRY_COUNT];
    };
};


constexpr void GlobalDescriptorTable::GDT32::SegmentDescriptor::set_segment_address(uint32_t segment) {
    segment_low  = segment & 0xFFFF;
    segment_high = (segment >> 16) & 0xF;
}

constexpr void GlobalDescriptorTable::GDT32::SegmentDescriptor::set_base_address(uint32_t base) {
    base_low  = base & 0xFFFF;
    base_mid  = (base >> 16) & 0xFF;
    base_high = base >> 24;
}

constexpr GlobalDescriptorTable::GDT32::SegmentDescriptor::SegmentDescriptor(
    uint32_t segment,
    uint32_t base,
    uint8_t  privilege,
    bool     code_descriptor
) {
    access.bitfield.non_system = 1;
    access.bitfield.type_bit   = (code_descriptor) ? 0xA : 0x2;
    access.bitfield.privilege  = privilege;
    access.bitfield.valid_bit  = 1;
    opr_32_bit  = 1;
    long_mode   = 0;
    granularity = 1;
    set_segment_address(segment);
    set_base_address(base);
}

constexpr GlobalDescriptorTable::GDT32::SegmentDescriptor::SegmentDescriptor() :
    access{ byte: 0 }, long_mode{0}, opr_32_bit{0}, granularity{0}
{
    set_segment_address(0);
    set_base_address(0);
}





static_assert(sizeof(GlobalDescriptorTable::GDT32::SegmentDescriptor) == 8);

#endif