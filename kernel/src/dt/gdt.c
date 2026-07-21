#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../../include/gdt.h"

extern void gdt_flush(uint32_t);

GDT_entry_table gdt_entries[6];
GDT_ptr pgdt;

void gdt_init() {
    pgdt.limit = (sizeof(GDT_entry_table) * 6) - 1;
    pgdt.base = (uint32_t)&gdt_entries;

    encode_gdt_entry(&gdt_entries[0], 0, 0, 0, 0); // null segment
    encode_gdt_entry(&gdt_entries[1], 0xFFFFFFFF, 0, 0x9A, 0xCF); // kernel code segment
    encode_gdt_entry(&gdt_entries[2], 0xFFFFFFFF, 0, 0x92, 0xCF); // kernel data segment
    encode_gdt_entry(&gdt_entries[3], 0xFFFFFFFF, 0, 0xFA, 0xCF); // user code segment
    encode_gdt_entry(&gdt_entries[4], 0xFFFFFFFF, 0, 0xF2, 0xCF); // user data segment

    gdt_flush((uint32_t)&pgdt);
}

void encode_gdt_entry(GDT_entry_table* table, uint32_t limit, uint32_t base, 
                      uint8_t access_byte, uint8_t flags) 
{
    table->limit_low = limit & 0xFFFF;
    table->base_low = base & 0xFFFF;
    table->base_mid = (base >> 16) & 0xFF;
    table->access_byte = access_byte & 0xFF;
    table->limit_high = (limit >> 16) & 0xF;
    table->flags = (flags >> 4) & 0xF;
    table->base_high = (base >> 24) & 0xFF;
}