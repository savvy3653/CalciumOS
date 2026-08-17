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

extern uint8_t floppy_init(void);
extern void ata_init(void);
extern void floppy_read_sector(uint32_t lba, uint8_t* buf);
extern void ata_read_sector(uint32_t lba, uint8_t* buf);
extern void ata_write_sector(uint32_t lba, uint8_t* buf);
extern uint16_t vfs_init(void (*read_sector)(uint32_t, uint8_t*));
extern void fat12_read_file(const char* fname, uint16_t mode);
extern void fat12_write_file(const char* fname, const char* data, uint16_t mode);

/* Check if the compiler thinks you are targeting the wrong operating system. */
// #if defined(__linux__)
// #error "You are not using a cross-compiler, you will most certainly run into trouble"
// #endif

/* This tutorial will only work for the 32-bit ix86 targets. */
// #if !defined(__i386__)
// #error "This tutorial needs to be compiled with a ix86-elf compiler"
// #endif

#define FLOPPY 0x8814
#define ATA    0x1818

#define FAT12 0x2912
#define FAT16 0x2916
#define EXT2  0xEF53

extern bool boot_mode;

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
    ata_init();

    void (*read_file)(const char*, uint16_t);
    void (*write_file)(const char*, const char*, uint16_t);
    uint16_t fs = vfs_init(ata_read_sector);
    switch (fs) {
        case FAT12:
            read_file = fat12_read_file;
            write_file = fat12_write_file;
            break;
        case FAT16:
            break;
        case EXT2:
            break;
        default:
            read_file = NULL;
            break;
    }
    ksleep(1000);

    boot_mode = false;
    cls();
	kprintf(" SheetOS System Release 0.2 (gcc-15.2.0)\n");
	kprintf(" Copyright (C) 2026 savvy3653\n");
	kprintf(" All rights reserved.\n");
	update_cursor(vga_column, vga_row+1);

    
    write_file("README.TXT", "metallica govno.", ATA);
    read_file("README.TXT", ATA);
    //read_file("HELLO.TXT", ata_read_sector);
    //read_file("READM2.TXT");
    //read_file("READM3.TXT");

	hang();
}
