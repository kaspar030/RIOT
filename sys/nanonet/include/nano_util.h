#ifndef NANO_UTIL_H
#define NANO_UTIL_H

#include <stdint.h>

uint16_t nano_util_calcsum(uint16_t csum, const uint8_t *buffer, size_t len);

void nano_util_addr_dump(uint8_t *addr, size_t len);

static inline char* nano_bufpos(uint8_t *buf, size_t len, size_t needed) {
    return (char *)(buf + len - needed);
}

static inline int nano_min(int a, int b) {
    return a < b ? a : b;
}

#endif /* NANO_UTIL_H */
