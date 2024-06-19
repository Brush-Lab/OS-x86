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

/**
 * So, uh sebagian besar kodenya berasal dari OS-2024
 */
static Paging::PageManagerState page_manager_state = {
    .page_frame_map = {[0 ... Paging::PAGE_FRAME_MAX_COUNT-1] = false}, 
    .free_page_frame_count = PAGE_FRAME_MAX_COUNT
};

void Paging::flush_single_tlb(void *virtual_addr){
    asm volatile("invlpg (%0)" : /* <Empty> */ : "b"(virtual_addr): "memory");
}

void Paging::update_page_directory_entry(
    struct PageDirectory *page_dir, 
    void *physical_addr, 
    void *virtual_addr, 
    struct PageDirectory::PageDirectoryEntry::PageDirectoryEntryFlag flag //I don't know why for this part fr. 
){
    /**
     * using reinterpret_cast<>
     * reinterpret_cast is a type of casting operator used in c++. 
     * - It is used to convert a pointer of some data type into a pointer of another data type, even if the data types before and after conversion are different.
     * - It does not check if the pointer type and data pointed by the pointer is same or not.
     * 
     */
    uint32_t page_index = (reinterpret_cast<uint32_t>(virtual_addr)>> 22) & 0x3FF; 
    page_dir->table[page_index].flag                     = flag;
    page_dir->table[page_index].lower_address            = (reinterpret_cast<uint32_t>(physical_addr) >> 2) & 0x3FF;
    flush_single_tlb(virtual_addr); 
}


/* --- Memory Management ---*/

bool Paging::paging_allocate_check(uint32_t amount){
    return page_manager_state.free_page_frame_count * PAGE_FRAME_MAX_COUNT >= amount;
}

bool Paging::paging_allocate_user_page_frame(struct Paging::PageDirectory *page_dir, void *virtual_addr){
    for(uint32_t i = 0; i < PAGE_FRAME_MAX_COUNT; i++){
        if(page_manager_state.page_frame_map[i] == false){
            page_manager_state.free_page_frame_count--; 
            void* physical_addr = reinterpret_cast<void*>(i * PAGE_FRAME_SIZE);
            struct PageDirectory::PageDirectoryEntry::PageDirectoryEntryFlag flag = {
                .present_bit = 1, 
                .write_bit = 1, 
                .use_pagesize_4_mb = 1, 
            };
            update_page_directory_entry(page_dir, physical_addr, virtual_addr, flag);
            return true;
        }
    }
    return false;
}

bool Paging::paging_free_user_page_frame(struct PageDirectory *page_dir, void *virtual_addr){
    
    uint32_t page_index = (reinterpret_cast<uint32_t>(virtual_addr) >> 22) & 0x3FF;
    uint32_t physical_addr = (page_dir->table[page_index].lower_address << 22);
    uint32_t frame_index = physical_addr / PAGE_FRAME_SIZE;

    if (page_manager_state.page_frame_map[frame_index]) {
        page_manager_state.page_frame_map[frame_index] = false;
        page_manager_state.free_page_frame_count++;
        page_dir->table[page_index].flag.present_bit = 0;
        flush_single_tlb(virtual_addr);
        return true;
    }
    return false;
}

