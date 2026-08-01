#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// FLAGS
#define FREE 0
#define USED 1
#define RESERVED 2

#define HEAP_BASE   0x10000000  // 256 MiB
#define HEAP_END    0x20000000  

extern uintptr_t __kernel_physical_start; 
extern uintptr_t __kernel_virtual_start;
extern uintptr_t __kernel_physical_end;
extern uintptr_t __kernel_virtual_end;

extern uint32_t page_directory[1024] __attribute__((aligned(4096)));
extern uint32_t page_table[1024] __attribute__((aligned(4096)));

typedef struct {
    uintptr_t paddr;
    uint8_t flags;
    uint16_t ref_count;
} PMM_block;


typedef struct {
    //uintptr_t vstart;
    //uintptr_t pstart;
    uint32_t size;
    //uint32_t flags;
    bool used;
} VMM_block;


typedef struct HEAP_block {
    intptr_t vaddr;
    size_t size;
    bool free;
    struct HEAP_block* next;
} HEAP_block;
extern HEAP_block* heap_blocks;

// Physical memory manager
void pmm_init();    // 
intptr_t pmm_alloc_block(); // returns phys address
int16_t pmm_find_free_block(); // return index in pmm_blocks
void pmm_free_block(int16_t index);


// Virtual memory manager
void vmm_init();
intptr_t vmm_alloc_block(size_t size, uint32_t flags); // returns virt address
int16_t vmm_find_free_block(); // returns index in page table
void vmm_free_block(intptr_t vaddr);
static inline void tlb_flush(intptr_t addr) {
    asm volatile("invlpg (%0)" :: "r"(addr) : "memory");
}
void heap_init();

void* kmalloc(size_t size);
void kfree(void* ptr);
