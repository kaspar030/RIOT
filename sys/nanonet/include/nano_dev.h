#ifndef NANO_DEV_H
#define NANO_DEV_H

#include <stdint.h>
#include <stddef.h>

#include "dev_eth_autoinit.h"

typedef struct nano_ctx nano_ctx_t;

typedef struct nano_dev {
    uint8_t mac_addr[6];
    uint32_t ipv4;
/*    uint8_t ipv6_ll[16];
    uint8_t ipv6_global[16];*/
    void*ptr;
    int (*send)(struct nano_dev *dev, uint8_t* dest_mac, uint16_t ethertype, uint8_t *buf, size_t len, size_t used);
    int (*send_raw)(struct nano_dev *dev, uint8_t* buf, size_t len);
    int (*reply)(nano_ctx_t *ctx);
    int (*l2_needed)(struct nano_dev *dev);
    void (*handle_isr)(void*);
} nano_dev_t;

enum {
    NANO_DEV_ETH = NUM_DEV_ETH-1,
    NANO_NUM_DEVS
};

void nanonet_init_devices(void);

extern nano_dev_t nanonet_devices[NANO_NUM_DEVS];
extern volatile unsigned nanonet_iflags;

#endif /* NANO_DEV_H */
