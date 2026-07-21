#include "../../include/stdlib.h"

void* memmove(void* dest, const void* src, size_t N){
    unsigned char* pd = (unsigned char*) dest;
	const unsigned char* ps = (const unsigned char*)src;
	if (pd < src) {
		for (size_t i = 0; i < N; i++)
			pd[i] = ps[i];
	} else {
		for (size_t i = N; i != 0; i--)
			pd[i-1] = ps[i-1];
	}
	return pd;
}