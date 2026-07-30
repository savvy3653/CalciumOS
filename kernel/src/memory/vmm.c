#include "../../include/memory.h"
#include "../../include/stdlib.h"

extern void set_page_dir(unsigned int*);
//extern void tlb_flush(intptr_t);
//VMM_block* vmm_blocks;

uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t page_table[1024] __attribute__((aligned(4096)));

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

// TODO: use size and figure out what the hell i should do with it / handle present pt
intptr_t vmm_alloc_block(size_t size, uint32_t flags) {
    intptr_t paddr = pmm_alloc_block();
    if (paddr == ERRCODE) return ERRCODE;

    intptr_t vaddr = paddr + 0xC0000000;

    uint32_t pdindex = (uint32_t)vaddr >> 22;
    uint32_t ptindex = ((uint32_t)vaddr >> 12) & 0x03FF;

    uint32_t* pd = (uint32_t*)0xFFFFF000;

    if (!(pd[pdindex] & 0x01)) {
        intptr_t new_pt_phys = pmm_alloc_block();
        if (new_pt_phys == ERRCODE) return ERRCODE;

        pd[pdindex] = ((uint32_t)new_pt_phys) | 0x03;

        uint32_t* new_pt = (uint32_t*)(0xFFC00000 + (pdindex * 0x1000));
        tlb_flush(new_pt);
        memset(new_pt, 0, 1024);
    }

    uint32_t* pt = (uint32_t*)(0xFFC00000 + (pdindex * 0x1000));
    pt[ptindex] = ((uint32_t)paddr) | (flags & 0xFFF) | 0x01;

    tlb_flush(vaddr);

    return vaddr;
}



