#include "../../include/idt.h"
#include "../../include/pic.h"
#include "../../include/vga.h"

extern void idt_flush(uint32_t);

IDT_entry_table idt_entries[256];
IDT_ptr pidt;

const char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment not present",
    "Stack fault",
    "General protection fault",
    "Page fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Fault",
    "Machine Check", 
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void idt_init() {
    pidt.size = sizeof(idt_entries) - 1;
    pidt.offset = (uint32_t)&idt_entries;
    memset(idt_entries, 0, sizeof(idt_entries) - 1);

    //0x20 commands and 0x21 data
    //0xA0 commands and 0xA1 data
    outb(PIC1, 0x11);
    outb(PIC2, 0x11);

    outb(0x21, PIC1);
    outb(0xA1, 0x28);

    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    encode_idt_entry(&idt_entries[0], (uint32_t)&isr0, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[1], (uint32_t)&isr1, 0x08, 0xF, 0, 1);
    encode_idt_entry(&idt_entries[2], (uint32_t)&isr2, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[3], (uint32_t)&isr0, 0x08, 0xF, 0, 1);
    encode_idt_entry(&idt_entries[4], (uint32_t)&isr0, 0x08, 0xF, 0, 1);
    encode_idt_entry(&idt_entries[5], (uint32_t)&isr0, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[6], (uint32_t)&isr0, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[7], (uint32_t)&isr7, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[8], (uint32_t)&isr8, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[9], (uint32_t)&isr9, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[10], (uint32_t)&isr10, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[11], (uint32_t)&isr11, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[12], (uint32_t)&isr12, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[13], (uint32_t)&isr13, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[14], (uint32_t)&isr14, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[15], (uint32_t)&isr15, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[16], (uint32_t)&isr16, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[17], (uint32_t)&isr17, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[18], (uint32_t)&isr18, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[19], (uint32_t)&isr19, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[20], (uint32_t)&isr20, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[21], (uint32_t)&isr21, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[22], (uint32_t)&isr22, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[23], (uint32_t)&isr23, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[24], (uint32_t)&isr24, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[25], (uint32_t)&isr25, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[26], (uint32_t)&isr26, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[27], (uint32_t)&isr27, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[28], (uint32_t)&isr28, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[29], (uint32_t)&isr29, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[30], (uint32_t)&isr30, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[31], (uint32_t)&isr31, 0x08, 0xE, 0, 1);

    encode_idt_entry(&idt_entries[32], (uint32_t)&irq0, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[33], (uint32_t)&irq1, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[34], (uint32_t)&irq2, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[35], (uint32_t)&irq3, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[36], (uint32_t)&irq4, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[37], (uint32_t)&irq5, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[38], (uint32_t)&irq6, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[39], (uint32_t)&irq7, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[40], (uint32_t)&irq8, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[41], (uint32_t)&irq9, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[42], (uint32_t)&irq10, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[43], (uint32_t)&irq11, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[44], (uint32_t)&irq12, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[45], (uint32_t)&irq13, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[46], (uint32_t)&irq14, 0x08, 0xE, 0, 1);
    encode_idt_entry(&idt_entries[47], (uint32_t)&irq15, 0x08, 0xE, 0, 1);

    encode_idt_entry(&idt_entries[128], (uint32_t)&isr128, 0x08, 0xE, 3, 1);// 0x80 int for syscalls

    idt_flush((uint32_t)&pidt);
}

void isr_handler(INT_registers* regs) {
    if (regs->int_no < 32) {
        kprintln(exception_messages[regs->int_no]);
        kprintln("Exception! System halted!");
        switch (regs->int_no)
        {
        case 14:
            break;
        default:
            break;
        }
        kpanic();
    }
}

void irq_handler(INT_registers* regs) {
    if (regs->int_no < 32) {
        kprintln(exception_messages[regs->int_no]);
        kprintln("Exception! System halted!");
        switch (regs->int_no)
        {
        case 14:
            break;
        default:
            break;
        }
        kpanic();
    }
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

