#ifndef _PAGING_HPP
#define _PAGING_HPP 

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


namespace Paging{
    /**
     * macro, tapi jadi const? gatau lihat contoh gdt.hpp
     * constexpr: variable ini constant dan bisa di-evaluasi pas compile-time
     */
    constexpr uint32_t PAGE_ENTRY_COUNT = 1024; 
    constexpr uint32_t PAGE_FRAME_MAX_COUNT = 32; 
    constexpr uint32_t PAGE_FRAME_UNMAPPED = 0xFF;
    constexpr uint32_t PAGE_FRAME_SIZE = (1 << (2 + 10 + 10));


    struct PageDirectoryEntryFlag; 
    struct PageDirectoryEntry; 
    extern struct PageDirectory _paging_kernel_page_directory; 

    /**
     * This code below I wrote it based on gdt.hpp. 
     * I hate bare metal.
     */
    struct [[gnu::packed]] PageDirectory{
        struct [[gnu::packed]] PageDirectoryEntry{
            struct [[gnu::packed]] PageDirectoryEntryFlag {
                uint8_t present_bit             : 1; 
                uint8_t write_bit               : 1; //Read/Write bit 
                uint8_t user_supervisor         : 1; //user/supervisor
                uint8_t pwt                     : 1; 
                uint8_t pcd                     : 1; // 'Cache Disabled' kalo bit-nya di-set dia bakalan cache, kalo gak di set ya gak cache.
                uint8_t accessed                : 1;
                uint8_t dirty                   : 1;
                uint8_t use_pagesize_4_mb       : 1; 
            } flag;
            uint16_t global_page    : 1;
            uint16_t ignored        : 3; // Ignored
            uint16_t pat            : 1; // PAT
            uint16_t higher_address : 4; // Bits 39:32 of address
            uint16_t reserved_2     : 5; // Reserved bit
            uint16_t lower_address  : 10; // Bits 31:22 of address 
        } table[PAGE_ENTRY_COUNT];
    };

    struct [[gnu::packed]] PageManagerState{
        bool page_frame_map[PAGE_FRAME_MAX_COUNT];
        uint32_t free_page_frame_count; 
    };

    /**
     * I stole it from OS-2024 h3h3.
     * 
     * @param page_dir      Page directory to update
     * @param physical_addr Physical address to map
     * @param virtual_addr  Virtual address to map
     * @param flag          Page entry flags
     */
    void update_page_directory_entry(
        struct PageDirectory *page_dir,
        void *physical_addr,
        void *virtual_addr, 
        struct PageDirectoryEntryFlag flag
    );

    void flush_single_tlb(void *virtual_addr); 

    /* --- Memory Management --- */

    /**
     * this piece of code, i stole from OS-2024. just Hope this work well.
     */
    bool paging_allocate_check(uint32_t amount);

    bool paging_allocate_user_page_frame(struct PageDirectory *page_dir, void *virtual_addr);

    bool paging_free_user_page_frame(struct PageDirectory *page_dir, void *virtual_addr);
};

extern struct Paging::PageDirectory _paging_kernel_page_directory;

#endif