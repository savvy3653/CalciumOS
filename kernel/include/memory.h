#include <stdint.h>
#include <stdbool.h>

// FLAGS
#define FREE 0
#define USED 1
#define RESERVED 2

#define HEAP_BASE   0x10000000  // 256 MiB
#define HEAP_END    0x30000000  // 768 MiB (131 072 chunks of 4KiB blocks??)

extern uintptr_t __kernel_physical_start; 
extern uintptr_t __kernel_virtual_start;
extern uintptr_t __kernel_physical_end;
extern uintptr_t __kernel_virtual_end;

typedef struct {
    uintptr_t paddr;
    uint8_t flags;
    uint16_t ref_count;
} PMM_block;

typedef struct {
    uintptr_t vstart;
    uintptr_t pstart;
    uint32_t size;
    uint32_t flags;
    bool used;
} VMM_block;

// Physical memory manager
void pmm_init();    // 
uintptr_t pmm_alloc_block(); // returns phys address
int16_t pmm_find_free_block(); // return index in pmm_blocks
void pmm_free_block();


// Virtual memory manager
void vmm_init();
uintptr_t vmm_alloc_block(); // returns virt address
int16_t vmm_find_free_block(); // returns index in page table
void vmm_free_block();
