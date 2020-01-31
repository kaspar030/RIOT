#ifndef NANO_UTIL_H
#define NANO_UTIL_H

#include <stdint.h>
#include "iolist.h"

uint16_t nano_util_calcsum(uint32_t csum, const uint8_t *buffer, size_t len);
uint16_t nano_util_calcsum_iolist(uint32_t sum, const iolist_t *iolist);

void nano_util_addr_dump(uint8_t *addr, size_t len);

static inline char* nano_bufpos(uint8_t *buf, size_t len, size_t needed) {
    return (char *)(buf + len - needed);
}

static inline int nano_min(int a, int b) {
    return a < b ? a : b;
}

static inline size_t nano_ctx_bufleft(nano_ctx_t *ctx, void *buf_pos)
{
    return (size_t)(ctx->buf - (size_t)buf_pos + ctx->len);
}

void nano_dump(uint8_t *addr, size_t len);

#endif /* NANO_UTIL_H */
