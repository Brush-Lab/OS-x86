#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <header/memory/paging4.hpp> 

using namespace Paging; 

/**
 * karena ada dua 
 * 1. _paging_kernel_page_directory untuk handle PageDirectory
 * 2. kernel_page_table untuk handle PageTable 
 * /

/** 
 * _paging_kernel_page_directory nantinya akan menunjuk ke kernel_page_table 
 * berdasarkan ilustrasi yang ada di kitab intel volume 3A 
 * figure 4.2 
 * Page Directory itu tiap entries-nya memiliki 4MB virtual memory yang menggunakan 4KB pages 
 * (1024 * 4KB) = 4MB 
 */
__attribute__((aligned(0x1000))) struct Paging::PageDirectory Paging::_paging_kernel_page_directory = {
    .table = {
        [0] = { 
            .flag = {
                .present_bit        = 1, 
                .write_bit          = 1, 
                .page_size          = 0, 
            }, 
            .page_table_base_addr = 0, 
        },
        [0x300] = {
            .flag = {
                .present_bit        = 1, 
                .write_bit          = 1, 
                .page_size          = 0, 
            },
            .page_table_base_addr = 0,  
        }
    }
};

/**
 * bagian ini bakalan nunjuk physical memory 
 * ilustasi ada di kitab intel volume 3A
 * 
 * figure 4.2
 */
__attribute__((aligned(0x1000))) struct Paging::PageTable Paging::kernel_page_table = {
    /**
     * part ini untuk mapping table ke physical memory
     */
    .table = {
        [0] = { //first 4KB
            .flag = {
                .present_bit        = 1, 
                .write_bit          = 1,
            }, 
            .page_base_addr = 0, //mapping physical memory 0x00000000
        },
        [1] = { //second 4Kb
            .flag = {
                .present_bit        = 1, 
                .write_bit          = 1,
            }, 
            .page_base_addr = 1, //mapping physical memory 0x00000004 (4KB)
        }
    }
};

static Paging::PageManagerState page_manager_state = {
    .page_frame_map = {[0 ... Paging::PAGE_FRAME_MAX_COUNT-1] = false}, 
    .free_page_frame_count = PAGE_FRAME_MAX_COUNT
};

/**
 * gatau, semoga ini masih sama. 
 */
void Paging::flush_single_tlb(void *virtual_addr){
    asm volatile("invlpg (%0)" : /* <Empty> */ : "b"(virtual_addr): "memory");
}

/**
 * update_page_driectory_entry: 
 * fungsi ini ada untuk mengupdate PageDirectory yang ada. 
 * 
 * PDE: Page Directory Entry
 */
void Paging::update_page_directory_entry(
    struct PageDirectory *page_dir, 
    struct PageTable *page_table,
    void *virtual_addr, 
    struct PageDirectory::PageDirectoryEntry::PageDirectoryEntryFlag flag
){
    uint32_t page_index = (reinterpret_cast<uint32_t>(virtual_addr) >> 22) & 0x3FF; 
    page_dir->table[page_index].flag = flag;
    page_dir->table[page_index].page_table_base_addr = (reinterpret_cast<uint32_t>(page_table) >> 12) & 0xFFFFF;
}

/**
 * update_page_table_entry: 
 * fungsi ini untuk mengupdate PageTable
 * 
 * PTE: Page Table Entry 
 */
void Paging::update_page_table_entry(
    struct PageTable *page_table,
    void *physical_addr,
    void *virtual_addr, 
    struct PageTable::PageTableEntry::PageTableEntryFlag flag
){
    uint32_t page_index = (reinterpret_cast<uint32_t>(virtual_addr) >> 12) & 0x3FF; 
    page_table->table[page_index].flag = flag;
    page_table->table[page_index].page_base_addr = (reinterpret_cast<uint32_t>(physical_addr) >> 12) & 0xFFFFF;
    flush_single_tlb(virtual_addr);
}

/* --- Memory Management ---*/

bool Paging::paging_allocate_check(uint32_t amount){
    return page_manager_state.free_page_frame_count * PAGE_FRAME_MAX_COUNT >= amount;
}

/**
 * di bagian ini akan dialokasikan paging pada user
 */
bool Paging::paging_allocate_user_page_frame(struct Paging::PageDirectory *page_dir, void *virtual_addr){
    for(uint32_t i = 0; i < PAGE_FRAME_MAX_COUNT; i++){
        if(page_manager_state.page_frame_map[i] == false){
            page_manager_state.free_page_frame_count--; 
            void* physical_addr = reinterpret_cast<void*>(i * PAGE_FRAME_SIZE);
            struct PageTable::PageTableEntry::PageTableEntryFlag flag = {
                .present_bit = 1, 
                .write_bit = 1, 
            };

            struct PageDirectory::PageDirectoryEntry::PageDirectoryEntryFlag dir_flag = {
                .present_bit = 1, 
                .write_bit = 1, 
                .page_size = 0, // diatur menjadi 0 untuk 4-KBytes Paging
            };

            /**
             * baik PTE ataupun PDE akan di-update dengan memanggil fungsi yang sudah disediakan 
             * */ 
            update_page_table_entry(&kernel_page_table, physical_addr, virtual_addr, flag);
            update_page_directory_entry(page_dir, &kernel_page_table, virtual_addr, dir_flag);
            return true;
        }
    }
    return false;
}

/**
 * asumsikan sama kayak 4-MBytes user 
 */
bool Paging::paging_free_user_page_frame(struct PageDirectory *page_dir, void *virtual_addr){
    uint32_t page_index = (reinterpret_cast<uint32_t>(virtual_addr) >> 22) & 0x3FF;
    uint32_t table_index = (reinterpret_cast<uint32_t>(virtual_addr) >> 12) & 0x3FF;
    uint32_t physical_addr = (kernel_page_table.table[table_index].page_base_addr << 12);
    uint32_t frame_index = physical_addr / PAGE_FRAME_SIZE;

    if (page_manager_state.page_frame_map[frame_index]) {
        page_manager_state.page_frame_map[frame_index] = false;
        page_manager_state.free_page_frame_count++;
        kernel_page_table.table[table_index].flag.present_bit = 0;
        flush_single_tlb(virtual_addr);
        return true;
    }
    return false;
}
