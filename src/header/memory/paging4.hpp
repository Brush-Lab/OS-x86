#ifndef _PAGING_HPP
#define _PAGING_HPP 

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

namespace Paging {
    /**
     * PAGE-nya didapat dari PDE dan PTE 
     * 4MB / 4KB = 1024
     */
    constexpr uint32_t PAGE_ENTRY_COUNT = 1024; 
    constexpr uint32_t PAGE_FRAME_MAX_COUNT = 32; 
    constexpr uint32_t PAGE_FRAME_UNMAPPED = 0xFF;
    /**
     * setiap page frame memiliki besar 4KB 
     * atau sekitar (4096 Byte)
     */
    constexpr uint32_t PAGE_FRAME_SIZE = 4096; 

    struct PageTableEntryFlag; 
    struct PageDirectoryEntryFlag; 
    struct PageDirectoryEntry; 
    struct PageTableEntry;

    extern struct PageDirectory _paging_kernel_page_directory; 
    extern struct PageTable kernel_page_table;

    /**
     * so, um this is update? 
     * it from figure 4-4. 
     * page directory and page table 
     * 
     * 
     * base on figure 4.2 linear address transaltaion to a 4-KByte page using 32-Bit Paging 
     * PDE -> PTE -> Physical Address
     * 
     */
    /**
     * this part from page 4.4 
     * PDE: page table 
     */
    struct [[gnu::packed]] PageDirectory {
        struct [[gnu::packed]] PageDirectoryEntry {
            struct [[gnu::packed]] PageDirectoryEntryFlag {
                uint8_t present_bit             : 1; // [Bit 0]
                uint8_t write_bit               : 1; // Read/Write bit [Bit 1]
                uint8_t user_supervisor         : 1; // User/Supervisor [Bit 2]
                uint8_t pwt                     : 1; // Page-level Write-Through [Bit 3]
                uint8_t pcd                     : 1; // Page-level Cache Disable [Bit 4]
                uint8_t accessed                : 1; // Accessed [Bit 5]
                uint8_t reserved                : 1; // Ignored part 1 [Bit 6]
                uint8_t page_size               : 1; // Page Size (0 for 4KB) [Bit 7]
                uint8_t ignored                 : 4; // Ignored part 2
                uint32_t page_table_base_addr   : 20; // Address of page table [Bit 12-31]
            } flag;
            uint32_t page_table_base_addr     : 20;
        } table[PAGE_ENTRY_COUNT];
    };

    /**
     * this part from 
     * PTE: 4KB Page 
     * 
     * 
     * @param present_bit: reference to a page table 
     * @param write_bit: ensure write not allowed to 4-MByte region (section 4.6?)
     * @param user_supervisor: if 0, user-mode cant access 4-MByte region 
     * @param pwt: determines memory type 
     * @param accessed: indicates entry has been used for linear-address 
     * @param ignored: mbuh 
     * @param dirty:
     * MALAS NGELANJUTIN NULIS
     */
    struct [[gnu::packed]] PageTable {
        struct [[gnu::packed]] PageTableEntry {
            struct [[gnu::packed]] PageTableEntryFlag {
                uint8_t present_bit             : 1; 
                uint8_t write_bit               : 1; // Read/Write bit 
                uint8_t user_supervisor         : 1; // User/Supervisor
                uint8_t pwt                     : 1; // Page-level Write-Through
                uint8_t pcd                     : 1; // Page-level Cache Disable
                uint8_t accessed                : 1; // Accessed
                uint8_t dirty                   : 1; // Dirty
                uint8_t pat                     : 1; // Page Attribute Table
                uint8_t global_page             : 1; // Global page
                uint8_t ignored                 : 3; // Ignored bit
                uint32_t page_base_addr         : 20; // Address of 4KB page frame
            } flag;
            uint32_t page_base_addr           : 20;
        } table[PAGE_ENTRY_COUNT];
    };

    struct [[gnu::packed]] PageManagerState {
        bool page_frame_map[PAGE_FRAME_MAX_COUNT];
        uint32_t free_page_frame_count; 
    };

    void Paging::update_page_directory_entry(
    struct PageDirectory *page_dir, 
    struct PageTable *page_table,
    void *virtual_addr, 
    struct PageDirectory::PageDirectoryEntry::PageDirectoryEntryFlag flag
    );

    void update_page_table_entry(
        struct PageTable *page_table,
        void *physical_addr,
        void *virtual_addr, 
        struct PageTableEntryFlag flag
    );

    /**
     * Sementara fungsinya pake 4-MByte doeloe? gayakin tbh. 
     */
    void flush_single_tlb(void *virtual_addr); 

    bool paging_allocate_check(uint32_t amount);

    bool paging_allocate_user_page_frame(struct PageDirectory *page_dir, void *virtual_addr);

    bool paging_free_user_page_frame(struct PageDirectory *page_dir, void *virtual_addr);
};

extern struct Paging::PageDirectory _paging_kernel_page_directory;

#endif
