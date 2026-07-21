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


void encode_gdt_entry(GDT_entry_table* table, uint32_t limit, uint32_t base, 
                      uint8_t access_byte, uint8_t flags);
void gdt_init();
