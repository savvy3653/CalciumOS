#pragma once

#include <stddef.h>

size_t strlen(const char* str);
void* memset(void* dest, int num, size_t N);
void* memcpy(void* dest, const void* src, size_t N);
void* memmove(void* dest, const void* src, size_t N);