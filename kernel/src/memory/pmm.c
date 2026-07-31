#include "../../include/memory.h"
#include "../../include/vga.h"
#include "../../include/stdlib.h"

PMM_block pmm_blocks[0x20000]; // (HEAP_END - HEAP_BASE) / 4096

void pmm_init() {
    for (int i = 0; i < 0x20000; i++) {
        pmm_blocks[i].paddr = HEAP_BASE + i * 0x1000;
        pmm_blocks[i].flags = FREE;
        pmm_blocks[i].ref_count = 0;
    } 
}   

intptr_t pmm_alloc_block() {
    int16_t index = pmm_find_free_block();
    if (index == ERRCODE) {
        kprint("Memory is full!");
        return ERRCODE;
    }
    pmm_blocks[index].flags = USED;
    pmm_blocks[index].ref_count = 1;

    return pmm_blocks[index].paddr;
}

int16_t pmm_find_free_block() {
    for (int i = 0; i < 0x20000; i++) {
        if (pmm_blocks[i].flags == FREE) {
            return i;
        }
    }
    return ERRCODE; // no free blocks
}

void pmm_free_block(int16_t index) {
    pmm_blocks[index].flags = FREE;
    pmm_blocks[index].ref_count--;
}   
