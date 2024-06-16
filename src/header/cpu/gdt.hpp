#ifndef _GDT_HPP
#define _GDT_HPP

#include <stdint.h>

namespace GlobalDescriptorTable {
    const uint32_t GDT32_ENTRY_COUNT = 1024;
    struct [[gnu::packed]] GDT32 {
        struct [[gnu::packed]] SegmentDescriptor {
            uint16_t segment_low;
            uint16_t base_low;

            // Next 16-bit (Bit 32 to 47)
            uint8_t base_mid;
            struct [[gnu::packed]] AccessByte {
                uint8_t type_bit   : 4;
                uint8_t non_system : 1;
                uint8_t privilege  : 2;
                uint8_t valid_bit  : 1;
            } access;

            // Next 8-bit (Bit 48 to 55)
            uint8_t segment_high : 4;
            uint8_t _reserved    : 1;
            uint8_t long_mode    : 1;
            uint8_t opr_32_bit   : 1;
            uint8_t granularity  : 1;

            // Last 8-bit (Bit 56 to 63)
            uint8_t base_high;

            consteval void set_segment_address(const uint32_t segment);
            consteval void set_base_address(const uint32_t base);
        } table[GDT32_ENTRY_COUNT];
    };

    consteval GDT32::SegmentDescriptor gdt32_segment_descriptor_creator(
        uint32_t segment,
        uint32_t base,
        uint8_t  privilege,
        bool     code_descriptor
    );
};




static_assert(sizeof(GlobalDescriptorTable::GDT32::SegmentDescriptor) == 8);

#endif