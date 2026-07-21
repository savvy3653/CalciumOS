#include "../../include/idt.h"

extern void idt_flush(uint32_t);

IDT_entry_table idt_entries[256];
IDT_ptr pidt;

void idt_init() {
    pidt.size = sizeof(idt_entries) * 256 - 1;
    pidt.offset = (uint32_t)&idt_entries;

    encode_idt_entry(&idt_entries[0], (uint32_t)&idt_entries[0], 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[1], (uint32_t)&idt_entries[1], 0x08, 0xF, 0, 1);
    encode_idt_entry(&idt_entries[2], (uint32_t)&idt_entries[2], 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[3], (uint32_t)&idt_entries[3], 0x08, 0xF, 0, 1);
    encode_idt_entry(&idt_entries[4], (uint32_t)&idt_entries[4], 0x08, 0xF, 0, 1);
    encode_idt_entry(&idt_entries[5], (uint32_t)&idt_entries[5], 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[6], (uint32_t)&idt_entries[6], 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[7], (uint32_t)&idt_entries[7], 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[8], (uint32_t)&idt_entries[8], 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[9], (uint32_t)&idt_entries[9], 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[10], (uint32_t)&idt_entries[10], 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[11], (uint32_t)&idt_entries[11], 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[12], (uint32_t)&idt_entries[12], 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[13], (uint32_t)&idt_entries[13], 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[14], (uint32_t)&idt_entries[14], 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[15], (uint32_t)&idt_entries[15], 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[16], (uint32_t)&idt_entries[16], 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[17], (uint32_t)&idt_entries[17], 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[18], (uint32_t)&idt_entries[18], 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[19], (uint32_t)&idt_entries[19], 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[20], (uint32_t)&idt_entries[20], 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[21], (uint32_t)&idt_entries[21], 0x08, 0xE, 0, 1);

    for (int i = 22; i <= 255; i++) {
            encode_idt_entry(&idt_entries[i], (uint32_t)&idt_entries[i], 0x08, 0xE, 0, 1);
    }

    idt_flush((uint32_t)&pidt);
}

void encode_idt_entry(IDT_entry_table* entry, uint32_t offset, 
                      uint16_t segment_selector, uint8_t gate_type,
                      uint8_t dpl, uint8_t p) 
{
    entry->offset_low = offset & 0xFFFF;
    entry->segment_selector = segment_selector;
    entry->reserved = 0;
    entry->gate_type = gate_type & 0xF;
    entry->null = 0;
    entry->dpl = dpl & 0x7;
    entry->p = p & 0x1;
    entry->offset_high = (offset >> 16) & 0xFFFF;
}