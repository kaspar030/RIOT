#ifndef NANO_ETH_H
#define NANO_ETH_H

#include <stdint.h>

typedef struct __attribute__((packed)) eth_hdr {
    uint8_t dst[6];
    uint8_t src[6];
    uint16_t ethertype;
} eth_hdr_t;

void nano_eth_handle(nano_dev_t *dev, char *buf, int len);

#endif /* NANO_ETH_H */
