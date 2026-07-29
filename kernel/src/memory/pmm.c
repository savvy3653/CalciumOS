#include "../../include/memory.h"
#include "../../include/vga.h"
#include "../../include/stdlib.h"

PMM_block pmm_blocks[1024];

void pmm_init() {
    for (int i = 0; i < 1024; i++) {
        pmm_blocks[i].paddr = HEAP_BASE + i * 4096;
        pmm_blocks[i].flags = FREE;
        pmm_blocks[i].ref_count = 0;
    } 
}   

uintptr_t pmm_alloc_block() {
    int16_t index = pmm_find_free_block();
    if (index == -1) {
        kprint("Memory is full!");
        return ERRCODE;
    }
    pmm_blocks[index].flags = USED;
    pmm_blocks[index].ref_count = 1;

    return pmm_blocks[index].paddr;
}

int16_t pmm_find_free_block() {
    for (int i = 0; i < 1024; i++) {
        if (pmm_blocks[i].flags == FREE) {
            return i;
        }
    }
    return ERRCODE; // no free blocks
}
