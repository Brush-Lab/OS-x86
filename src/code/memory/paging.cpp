#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <header/memory/paging.hpp> 

using namespace Paging; 


/*===============INITIALIZE==============*/

/**
 * what kind of code i've been writing on? I've never seen such a code...
 * lord please help me 
 * I think it kinda simillar with paging.c in OS-2024. Yeah I think it is. 
 */
__attribute__((aligned(0x1000))) struct Paging::PageDirectory Paging::_paging_kernel_page_directory = {
    .table = {
        [0] = {
            .flag = {
                .present_bit        = 1, 
                .write_bit          = 1, 
                .use_pagesize_4_mb  = 1,
            }, 
            .lower_address          = 0,
        },
        [0x300] = {
            .flag = {
                .present_bit        = 1, 
                .write_bit          = 1, 
                .use_pagesize_4_mb  = 1, 
            },
            .lower_address          = 0, 
        }
    }
};

static Paging::PageManagerState page_manager_state = {
    .page_frame_map = {[0 ... Paging::PAGE_FRAME_MAX_COUNT-1] = false}, 
    .free_page_frame_count = PAGE_FRAME_MAX_COUNT
};

void Paging::update_page_directory_entry(
    struct PageDirectory *page_dir, 
    void *physical_addr, 
    void *virtual_addr, 
    struct PageDirectory::PageDirectoryEntry::PageDirectoryEntryFlag flag
){
    uint32_t page_index = ((uint32_t) virtual_addr >> 22) & 0x3FF; 
    page_dir->table[page_index].flag                     = flag;
    page_dir->table[page_index].lower_address            = ((uint32_t) physical_addr >> 2) & 0x3FF;
    flush_single_tlb(virtual_addr);
}

void flush_single_tlb(void *virtual_addr){
    asm volatile("invlpg (%0)" : /* <Empty> */ : "b"(virtual_addr): "memory");
}