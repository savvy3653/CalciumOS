#include "../../include/stdlib.h"

void kpanic() {
    asm volatile ("cli": : :"memory");
    while (true) {
        asm volatile ("hlt");
    }
}
