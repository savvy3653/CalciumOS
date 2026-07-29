#include "../../include/memory.h"

extern void set_page_dir(unsigned int*);

uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));

void vmm_init() {
    // paging init
    for (int i = 0; i < 1024; i++) {
        page_directory[i] = 0x00000002;
    }
    for (int i = 0; i < 1024; i++) {
        first_page_table[i] = (i * 0x1000) | 3;
    }

    // VGA mapping
    first_page_table[1023] = 0x000B8000 | 0x003;

    uint32_t pt_phys = (uint32_t)first_page_table - 0xC0000000;
    page_directory[0]   = pt_phys | 3;
    page_directory[768] = pt_phys | 3;

    set_page_dir((unsigned int*)((uint32_t)page_directory - 0xC0000000));
}