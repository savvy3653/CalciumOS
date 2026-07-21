#pragma once

#include <stddef.h>
#include <stdbool.h>

// basic implementations
size_t strlen(const char* str);
void* memset(void* dest, int num, size_t N);
void* memcpy(void* dest, const void* src, size_t N);
void* memmove(void* dest, const void* src, size_t N);
int printf(const char* restrict format, ...);
int memcmp(const void* ptr1, const void* ptr2, size_t N);


// custom 
static inline void kabort() {
    while (true)
        asm volatile ("hlt");
}
