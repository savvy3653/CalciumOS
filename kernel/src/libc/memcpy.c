#include "../../include/stdlib.h"

void* memcpy(void* dest, const void* src, size_t N) {
    unsigned char* pd = (unsigned char*)dest;
    unsigned char* ps = (unsigned char*)src;

    for (size_t i = 0; i < N; i++) {
        pd[i] = ps[i];
    }
    return pd;
}