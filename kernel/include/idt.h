#pragma once

#include <stdint.h>

#include "stdlib.h"

// IRQ's
#define INT_TIMER 0
#define INT_KEYBOARD 1
#define INT_CASCADE 2
#define INT_COM2 3
#define INT_COM1 4
#define INT_LPT2 5
#define INT_FLOPPY 6
#define INT_LPT1 7
#define INT_CMOS 8
#define INT_RESERVED0 9     // Free for peripherals / legacy SCSI / NIC 
#define INT_RESEDVED1 10    // Free for peripherals / SCSI / NIC 
#define INT_RESERVED2 11    // Free for peripherals / SCSI / NIC 
#define INT_PS2MOUSE 12
#define INT_FPUCOPROCESSOR 13
#define INT_PRIMARYATA 14
#define INT_SECONDARYATA 15

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
void irq_install_routine(uint8_t irq, void (*handler)(INT_registers* regs));
void irq_uninstall_routine(uint8_t irq);
void isr_handler(INT_registers* regs);
void irq_handler(INT_registers* regs);
void idt_init();

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void isr128(); // syscalls

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();
