#ifndef NANO_ETH_H
#define NANO_ETH_H

#include <stdint.h>

#include "net/nano/dev.h"
#include "net/nano/ctx.h"

typedef struct __attribute__((packed)) eth_hdr {
    uint8_t dst[6];
    uint8_t src[6];
    uint16_t ethertype;
} eth_hdr_t;

void nano_eth_handle(nano_dev_t *dev, uint8_t *buf, size_t len);
int nano_eth_reply(nano_ctx_t *ctx);
void nano_eth_get_iid(uint8_t *eui64, const uint8_t *mac);

#endif /* NANO_ETH_H */
