#include "../../include/memory.h"
#include "../../include/stdlib.h"

extern void set_page_dir(uint32_t*);
//extern void tlb_flush(intptr_t);
//VMM_block* vmm_blocks;

uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t page_table[1024] __attribute__((aligned(4096)));

VMM_block pt_map[0x20000]; // (HEAP_END - HEAP_BASE) / 4096

void vmm_init() {
    for (int i = 0; i < 1024; i++) page_directory[i] = 0x00000002;
    for (int i = 0; i < 1024; i++) page_table[i] = (i * 0x1000) | 3;

    page_table[1023] = 0x000B8000 | 0x003; // VGA

    uint32_t pt_phys = (uint32_t)page_table - 0xC0000000;
    page_directory[0]   = pt_phys | 3;
    page_directory[768] = pt_phys | 3;

    // recursive mapping
    page_directory[1023] = ((uint32_t)page_directory - 0xC0000000) | 3;

    set_page_dir((unsigned int*)((uint32_t)page_directory - 0xC0000000));
}

// TODO: handle present pt
// BE CAREFUL WITH CUSTOM_VADDR!
intptr_t vmm_alloc_block(size_t size, uint32_t flags, uint32_t custom_vaddr) {
    uint16_t pt_count = (uint16_t)ceil(size, 0x1000);
    intptr_t start_vaddr = NULL;
    uint16_t pt_index = 0;

    // looking for free pt space
    for (int i = 0; i < 0x20000; i++) {
        bool free = false;
        for (int j = 0; j < pt_count; j++) {
            if (pt_map[j+i].used == true) break;
            if (pt_map[j+i].used == false && j + 1 == pt_count) free = true;
        }
        if (free) {
            pt_index = i;
            break;
        }
    }

    for (int i = 0; i < pt_count; i++) {
        intptr_t paddr = pmm_alloc_block();
        if (paddr == ERRCODE) return ERRCODE;

        intptr_t vaddr;
        if (custom_vaddr == NULL) {
            vaddr = (pt_index + i) * 0x1000 + VMM_HEAP_BASE;
        } else {
            vaddr = custom_vaddr;
        }

        uint32_t pdindex = (uint32_t)vaddr >> 22;
        uint32_t ptindex = ((uint32_t)vaddr >> 12) & 0x03FF;

        uint32_t* pd = (uint32_t*)0xFFFFF000;
        if (!(pd[pdindex] & 0x01)) {
            intptr_t new_pt_phys = pmm_alloc_block();
            if (new_pt_phys == ERRCODE) return ERRCODE;

            pd[pdindex] = ((uint32_t)new_pt_phys) | 0x03;

            uint32_t* new_pt = (uint32_t*)(0xFFC00000 + (pdindex * 0x1000));
            tlb_flush(new_pt);
            memset(new_pt, 0, 4096);
        }

        uint32_t* pt = (uint32_t*)(0xFFC00000 + (pdindex * 0x1000));
        pt[ptindex] = ((uint32_t)paddr) | (flags & 0xFFF) | 0x01;

        tlb_flush(vaddr);
        pt_map[pt_index + i].used = true;
        if (i == 0) 
            start_vaddr = vaddr;
    }
    return start_vaddr;
}


void heap_init() {
    const uint32_t metadata_base = 
        vmm_alloc_block(0x1000, 0x7, METADATA_HEAP_BASE); // not necessarily need to be a variable, you can just call it. (made for debug)
    intptr_t vaddr = vmm_alloc_block(0x1000, 0x7, NULL);

    heap_blocks = (HEAP_block*)metadata_base;
    heap_blocks[0].vaddr = vaddr;
    heap_blocks[0].size = 0x1000;
    heap_blocks[0].free = true;
    heap_blocks[0].next = NULL;
}


