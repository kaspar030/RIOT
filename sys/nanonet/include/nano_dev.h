#ifndef NANO_DEV_H
#define NANO_DEV_H

#include <stdint.h>
#include <stddef.h>

#include "nano_sndbuf.h"
#include "net/netdev2.h"

typedef struct nano_ctx nano_ctx_t;
typedef struct nano_dev nano_dev_t;

struct nano_dev {
    netdev2_t *netdev;
    int (*send)(struct nano_dev *dev, nano_sndbuf_t *buf, uint8_t* dest_mac, uint16_t ethertype);
    int (*send_raw)(struct nano_dev *dev, uint8_t* buf, size_t len);
    int (*reply)(nano_ctx_t *ctx);
    uint8_t l2_addr[6];
    uint16_t l2_needed;
    uint32_t ipv4;
    uint8_t ipv6_ll[16];
    uint8_t ipv6_global[16];
    void (*handle_isr)(nano_dev_t*);
};

extern const unsigned nano_dev_numof;
void nanonet_init_devices(void);
extern nano_dev_t nanonet_devices[];

#endif /* NANO_DEV_H */
