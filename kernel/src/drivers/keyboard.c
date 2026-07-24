#include "../../include/keyboard.h"
#include "../../include/stdlib.h"
#include "../../include/idt.h"
#include "../../include/pic.h"
#include "../../include/vga.h"

void keyboard_init() {
    irq_install_routine(1, &keyboard_handler);
}

void keyboard_handler(INT_registers* regs) {
    uint8_t bbyte = inb(0x60);
    kprintln("key!");
}