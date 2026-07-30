#include "../../include/memory.h"
#include "../../include/stdlib.h"

void* kmalloc(size_t size) {
    // 7 - PRESENT/RW/USER bits set
    intptr_t vaddr = vmm_alloc_block(size, 0x7);
    if (vaddr == ERRCODE) return NULL;
    return (void*)vaddr;
}
