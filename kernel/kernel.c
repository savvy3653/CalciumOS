#include "include/vga.h"
#include "include/memory.h"
#include "include/stdlib.h"
#include "include/stdio.h"
#include "include/gdt.h"
#include "include/idt.h"
#include "include/keyboard.h"

extern void hang(void);
extern void pit_init(uint32_t frq_hz);
extern void ksleep(uint32_t ms);

extern void floppy_init(void);
extern void floppy_detect_drives(void);
extern void floppy_read_sector(uint32_t lba, uint8_t* buf);
/* Check if the compiler thinks you are targeting the wrong operating system. */
// #if defined(__linux__)
// #error "You are not using a cross-compiler, you will most certainly run into trouble"
// #endif

/* This tutorial will only work for the 32-bit ix86 targets. */
// #if !defined(__i386__)
// #error "This tutorial needs to be compiled with a ix86-elf compiler"
// #endif

void kernel_main(void) {
	gdt_init();
	idt_init();
    pit_init(1000); // 1ms
    pmm_init();
	vmm_init();
    heap_init();
	vga_init();

	// interrupt handlers
	keyboard_init();
    floppy_detect_drives();
    floppy_init();

    cls();
	kprintf(" CalciumOS System Release 0.1 (gcc-15.2.0)\n");
	kprintf(" Copyright (C) 2026 savvy3653\n");
	kprintf(" All rights reserved.\n");
	update_cursor(vga_column, vga_row+1);

    uint8_t buffer[512];
    floppy_read_sector(2, buffer);
    
    // magic value
    kprintf("%x ", buffer[56]);
    kprintf("%x\n", buffer[57]);

	hang();
}
