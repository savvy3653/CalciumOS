// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 savvy3653 

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
extern void floppy_read_sector(uint32_t lba, uint8_t* buf);
extern uint16_t vfs_init(void);
extern void fat12_read_file(const char*);

/* Check if the compiler thinks you are targeting the wrong operating system. */
// #if defined(__linux__)
// #error "You are not using a cross-compiler, you will most certainly run into trouble"
// #endif

/* This tutorial will only work for the 32-bit ix86 targets. */
// #if !defined(__i386__)
// #error "This tutorial needs to be compiled with a ix86-elf compiler"
// #endif

#define FAT12 0x29
#define EXT2  0xEF53

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
    floppy_init();

    void (*read_file)(const char*);
    uint16_t fs = vfs_init();
    switch (fs) {
        case 0x29:
            read_file = fat12_read_file;
            break;
        case 0xEF53:
            break;
        default:
            read_file = NULL;
            break;
    }
    ksleep(1000);

    cls();
	kprintf(" SheetOS System Release 0.2 (gcc-15.2.0)\n");
	kprintf(" Copyright (C) 2026 savvy3653\n");
	kprintf(" All rights reserved.\n");
	update_cursor(vga_column, vga_row+1);

    read_file("README.TXT");
    read_file("ROBRTE.TXT");

	hang();
}
