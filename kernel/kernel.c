#include "include/vga.h"
#include "include/memory.h"
#include "include/stdlib.h"
#include "include/stdio.h"
#include "include/gdt.h"
#include "include/idt.h"
#include "include/keyboard.h"

extern void hang(void);
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
    pmm_init();
	vmm_init();
	vga_init();
	
	// interrupt handlers
	keyboard_init();

	kprintf(" CalciumOS System Release 0.1 (gcc-15.2.0)\n");
	kprintf(" Copyright (C) 2026 savvy3653\n");
	kprintf(" All rights reserved.\n");
	update_cursor(vga_column, vga_row+1);

	int* ptr = kmalloc(sizeof(int));
    *ptr = 1488;
    kprintf("%d\n", *ptr);

	hang();
}
