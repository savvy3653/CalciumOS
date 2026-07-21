#pragma once

#include <stdint.h>

typedef struct {
    uint16_t offset_low;
    uint16_t segment_selector;
    uint8_t reserved;
    uint8_t gate_type : 4;
    uint8_t null : 1;
    uint8_t dpl : 2;
    uint8_t p : 1;
    uint16_t offset_high;
} __attribute__((packed)) IDT_entry_table;

typedef struct {
    uint16_t size;
    uint32_t offset;
} __attribute__((packed)) IDT_ptr;

void encode_idt_entry(IDT_entry_table* entry, uint32_t offset, 
                      uint16_t segment_selector, uint8_t gate_type,
                      uint8_t dpl, uint8_t p);
void idt_init();
