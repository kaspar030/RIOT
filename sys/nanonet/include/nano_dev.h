#ifndef NANO_DEV_H
#define NANO_DEV_H

#include <stdint.h>
#include <stddef.h>

#include "nano_config.h"

#include "net/netdev.h"

typedef struct nano_ctx nano_ctx_t;
typedef struct nano_dev nano_dev_t;

#ifdef MODULE_NETDEV_IEEE80154
#define NANO_L2_ADDRLEN (8)
#else
#define NANO_L2_ADDRLEN (6)
#endif

typedef int (*nano_dev_send_t)(struct nano_dev *dev, const iolist_t *iolist, uint8_t* dest_mac, ...);

struct nano_dev {
    netdev_t *netdev;
    int (*send)(struct nano_dev *dev, const iolist_t *iolist, uint8_t* dest_mac, ...);
    int (*send_raw)(struct nano_dev *dev, uint8_t* buf, size_t len);
    int (*reply)(nano_ctx_t *ctx);
    uint8_t l2_addr[NANO_L2_ADDRLEN];
    uint16_t l2_needed;

#ifdef NANONET_IPV4
    uint32_t ipv4;
#endif

    uint8_t ipv6_ll[16];
    uint8_t ipv6_global[16];
    void (*handle_rx)(nano_dev_t*, uint8_t *buf, size_t nbytes);
};

extern const unsigned nano_dev_numof;
void nanonet_init_devices(void);
extern nano_dev_t nanonet_devices[];

#endif /* NANO_DEV_H */
