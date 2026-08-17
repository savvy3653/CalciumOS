#include "../../include/memory.h"
#include "../../include/stdlib.h"

HEAP_block* heap_blocks = (HEAP_block*)METADATA_HEAP_BASE;
//HEAP_block* heap_blocks_end = heap_blocks; // init

void* kmalloc(size_t size) {
    HEAP_block* it;
    intptr_t vaddr = 0;
    intptr_t last_node_vaddr = 0;

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
            it->next = (HEAP_block*)((intptr_t*)(it + sizeof(HEAP_block)));
            // init next node
            it->next->vaddr = it->vaddr + it->size; 
            it->next->size = prev_size - size;
            it->next->free = true;
            it->next->next = prev_next;

            vaddr = it->vaddr;
            break;
        }
        last_node_vaddr = (intptr_t)it;
        it = it->next;
    }
    // if no address fit, allocate new vm_block
    if (vaddr == 0) { 
        intptr_t new_block_vaddr = last_node_vaddr + sizeof(HEAP_block);
        // 7 - PRESENT/RW/USER bits set
        intptr_t block_vaddr = vmm_alloc_block(0x1000, 0x7, new_block_vaddr);
        vaddr = vmm_alloc_block(size, 0x7, 0);

        // create new heap_block
        HEAP_block* block = (HEAP_block*)block_vaddr;
        block[0].vaddr = vaddr;
        block[0].size = size;
        block[0].free = true;
        block[0].next = 0;
        
        // insert it in heap_blocks
        HEAP_block* it1 = &heap_blocks[0];
        while (it1->next != 0) {
            it1 = it1->next;
        }
        it1->next = block;

        vaddr = block[0].vaddr;
        it = block;
    }
    if (vaddr == ERRCODE) return 0;

    it->free = false;
    return (void*)vaddr;
}
