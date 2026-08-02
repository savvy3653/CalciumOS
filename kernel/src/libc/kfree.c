#include "../../include/memory.h"
#include "../../include/stdlib.h"

void kfree(void* ptr) {
    HEAP_block* it = &heap_blocks[0];
    while (it) {
        if (it->vaddr == ptr) {
            memset(ptr, 0, it->size);
            it->free = true;
            return;
        }
        it = it->next;
    }
    if (it == NULL) return;
}
