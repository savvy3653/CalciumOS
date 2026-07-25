#include "include/vga.h"
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
	//irq_install_routine();
	vga_init();
	
	keyboard_init();

	kprintln(" CalciumOS System Release 0.1 (gcc-15.2.0)");
	kprintln(" Copyright (C) 2026 savvy3653");
	kprintln(" All rights reserved.");
	update_cursor(vga_column, vga_row+1);
	int a = 8;
	kprintf("byte %d", a);

	hang();
}
