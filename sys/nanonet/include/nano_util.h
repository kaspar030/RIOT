#ifndef NANO_UTIL_H
#define NANO_UTIL_H

#include <stdint.h>

uint16_t nano_calcsum(uint16_t* addr, int count);

static inline char* nano_bufpos(char *buf, int len, int needed) {
    return (char *)(buf + len - needed);
}

static inline int nano_min(int a, int b) {
    return a < b ? a : b;
}

#endif /* NANO_UTIL_H */
