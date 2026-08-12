#include "../../include/stdlib.h"

void* memset(void* dest, int num, size_t N) {
    unsigned char* p = dest;
    for (size_t i = N; i > 0; i--) {
        *p = (unsigned char)num;
        p++;
    }
    return dest;
}
