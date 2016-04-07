#ifndef NANO_IEEE802154_H
#define NANO_IEEE802154_H

#include <stdint.h>

typedef struct __attribute__((packed)) ieee802154_hdr {
    void *pass;
} ieee802154_hdr_t;

void nano_ieee802154_handle(nano_dev_t *dev, uint8_t *buf, size_t len);
int nano_ieee802154_reply(nano_ctx_t *ctx);
void nano_ieee802154_get_iid(uint8_t *eui64, const uint8_t *mac);

#endif /* NANO_IEEE802154_H */
