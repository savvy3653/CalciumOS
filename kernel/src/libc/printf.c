#include <limits.h>

#include "../../include/stdlib.h"
#include "../../include/stdio.h"

void kprintf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buf[256]; // string buffer
    char* p = buf;
    int state = PRINTF_STATE_START;
    int length = PRINTF_LENGTH_START;
    int radix = 10;
    bool sign = false;

    while (*fmt) {
        switch (state) {
        case PRINTF_STATE_START:
            if (*fmt == '%') {
                state = PRINTF_STATE_LENGTH;
            } else {
                *p++ = *fmt;
            }
            break;
        case PRINTF_STATE_LENGTH:
            if (*fmt == 'h') {
                length = PRINTF_LENGTH_SHORT;
                state = PRINTF_STATE_SHORT;
            } else if (*fmt == 'l') {
                length = PRINTF_LENGTH_LONG;
                state = PRINTF_STATE_LONG;
            } else {
                state = PRINTF_STATE_SPEC;
                goto SPEC;
            }
            break;
        case PRINTF_STATE_SHORT:
            if (*fmt == 'h') {
                length = PRINTF_LENGTH_SHORT_SHORT;
            }
            state = PRINTF_STATE_SPEC;
            // fallthrough
        case PRINTF_STATE_LONG:
            if (state == PRINTF_STATE_LONG && *fmt == 'l') {
                length = PRINTF_LENGTH_LONG_LONG;
            }
            state = PRINTF_STATE_SPEC;
            // fallthrough
        case PRINTF_STATE_SPEC:
        SPEC:
            switch (*fmt) {
            case 'c':
                *p++ = (char)va_arg(args, int);
                break;
            case 's':
                {
                    const char* s = va_arg(args, const char*);
                    while (*s) *p++ = *s++;
                }
                break;
            case '%':
                *p++ = '%';
                break;
            case 'd': case 'i':
                sign = true; radix = 10;
                p += printf_number(p, args, length, sign, radix);
                break;
            case 'u':
                sign = false; radix = 10;
                p += printf_number(p, args, length, sign, radix);
                break;
            case 'x': case 'X': case 'p':
                sign = false; radix = 16;
                p += printf_number(p, args, length, sign, radix);
                break;
            case 'o':
                sign = false; radix = 8;
                p += printf_number(p, args, length, sign, radix);
                break;
            default:
                // unknown specifier. ignore.
                break;
            }
            // reset the condition
            state = PRINTF_STATE_START;
            length = PRINTF_LENGTH_START;
            radix = 10;
            sign = false;
            break;
        }
        fmt++;
    }
    *p = '\0';
    va_end(args);

    // string is ready. kprint
    kprint(buf);
}

const char possibleChars[] = "0123456789abcdef";

int printf_number(char* dest, va_list args, int length, bool sign, int radix) {
    uint32_t number = 0;
    int number_sign = 1;
    char tmp[32];
    int pos = 0;

    if (sign) {
        int32_t n = va_arg(args, int32_t);
        if (n < 0) { n = -n; number_sign = -1; }
        number = (uint32_t)n;
    } else {
        number = va_arg(args, uint32_t);
    }

    do {
        tmp[pos++] = possibleChars[number % radix];
        number /= radix;
    } while (number);

    if (sign && number_sign < 0) tmp[pos++] = '-';

    // reversing
    for (int i = pos - 1; i >= 0; --i) {
        *dest++ = tmp[i];
    }
    return pos;
}