#include "../../include/memory.h"
#include "../../include/stdlib.h"

// [heap_block0] ->| memory | [heap_block1] ->| memory | ...

HEAP_block* heap_blocks = (HEAP_block*)VMM_HEAP_BASE;
//HEAP_block* heap_blocks_end = heap_blocks; // init

void* kmalloc(size_t size) {
    HEAP_block* it;
    intptr_t vaddr = NULL;

    it = &heap_blocks[0];
    while (it) {
        if (it->size == size && it->free == true) {
            vaddr = it->vaddr;
            break;
        // if size of block is greater, separate vm_block
        } else if (it->size > size && it->free == true) {
            size_t prev_size = it->size;
            HEAP_block* prev_next = it->next;
            // init this node
            it->size = size;
            it->next = (HEAP_block*)((intptr_t*)(it->vaddr + it->size));
            // init next node
            it->next->vaddr = it->vaddr + it->size + sizeof(HEAP_block); 
            it->next->size = prev_size - size - sizeof(HEAP_block);
            it->next->free = true;
            it->next->next = prev_next;

            vaddr = it->vaddr;
            break;
        }
        it = it->next;
    }
    // if no address fit, allocate new vm_block
    if (vaddr == NULL) { 
        // 7 - PRESENT/RW/USER bits set
        vaddr = vmm_alloc_block(size, 0x7);

        // create new heap_block
        HEAP_block* block = (HEAP_block*)vaddr;
        block[0].vaddr = vaddr + sizeof(HEAP_block);
        block[0].size = size;
        block[0].free = true;
        block[0].next = NULL;
        
        // insert it in heap_blocks
        HEAP_block* it1 = &heap_blocks[0];
        while (it1->next != NULL) {
            it1 = it1->next;
        }
        it1->next = block;

        vaddr = block[0].vaddr;
        it = block;
    }
    if (vaddr == ERRCODE) return NULL;

    it->free = false;
    return (void*)vaddr;
}
