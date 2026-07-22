#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

// Interrupt registers (pushing on stack in idt.asm)
typedef struct {
    uint32_t cr2;
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, csm, eflags, useresp, ss;
} INT_registers;

// basic implementations
size_t strlen(const char* str);
void* memset(void* dest, int num, size_t N);
void* memcpy(void* dest, const void* src, size_t N);
void* memmove(void* dest, const void* src, size_t N);
int printf(const char* restrict format, ...);
int memcmp(const void* ptr1, const void* ptr2, size_t N);


//
// custom 
static inline void kpanic() {
    asm volatile ("cli": : :"memory");
    while (true) {
        asm volatile ("hlt");
    }
}

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %w1, %b0"
                   : "=a"(ret)
                   : "Nd"(port)
                   : "memory");
    return ret;
}

static inline void io_wait(void) {
    outb(0x80, 0);
}
