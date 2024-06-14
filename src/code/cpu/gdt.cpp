#include <header/cpu/gdt.hpp>

using namespace GlobalDescriptorTable;

consteval void GDT32::SegmentDescriptor::set_segment_address(const uint32_t segment) {
    this->segment_low  = segment & 0xFFFF;
    this->segment_high = (segment >> 16) & 0xF;
}

consteval void GDT32::SegmentDescriptor::set_base_address(const uint32_t base) {
    this->base_low  = base & 0xFFFF;
    this->base_mid  = (base >> 16) & 0xFF;
    this->base_high = base >> 24;
}

consteval GDT32::SegmentDescriptor gdt32_segment_descriptor_creator(
    uint32_t segment,
    uint32_t base,
    uint8_t  privilege,
    bool     code_descriptor
) {
    GDT32::SegmentDescriptor segment_descriptor;
    segment_descriptor.access.non_system = 1,
    segment_descriptor.access.type_bit   = (code_descriptor) ? 0xA : 0x2,
    segment_descriptor.access.privilege  = privilege,
    segment_descriptor.access.valid_bit  = 1,
    segment_descriptor.opr_32_bit        = 1,
    segment_descriptor.long_mode         = 0,
    segment_descriptor.granularity       = 1,
    segment_descriptor.set_segment_address(segment);
    segment_descriptor.set_base_address(base);
    return segment_descriptor;
}