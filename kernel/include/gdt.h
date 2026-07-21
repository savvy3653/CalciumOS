#pragma once

#include <stdint.h>

#if 0
typedef struct {
    uint8_t A : 1;
    uint8_t RW : 1;
    uint8_t DC : 1;
    uint8_t E : 1;
    uint8_t S : 1;
    uint8_t DPL : 2;
    uint8_t P : 1;
} __attribute__((packed)) Access_Byte;
 #endif

typedef struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    // Access_Byte access_byte;
    uint8_t access_byte;
    uint8_t limit_high : 4;
    uint8_t flags : 4;
    uint8_t base_high; 
} __attribute__((packed)) GDT_entry_table;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) GDT_ptr;

typedef struct {
    uint32_t prev_tss;
	uint32_t esp0;
	uint32_t ss0;
	uint32_t esp1;
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldt;
	uint32_t trap;
	uint32_t iomap_base;
} __attribute__((packed)) TSS_entry_table;

void encode_gdt_entry(GDT_entry_table* table, uint32_t limit, uint32_t base, 
                      uint8_t access_byte, uint8_t flags);
void gdt_init();
