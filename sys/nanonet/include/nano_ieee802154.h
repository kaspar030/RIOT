#ifndef NANO_IEEE802154_H
#define NANO_IEEE802154_H

#include <stdint.h>

typedef struct __attribute__((packed)) ieee802154_hdr {
    void *pass;
} ieee802154_hdr_t;

void nano_ieee802154_handle(nano_dev_t *dev, uint8_t *buf, size_t len);
int nano_ieee802154_reply(nano_ctx_t *ctx);
void nano_ieee802154_get_iid(const uint8_t *addr_in, size_t addr_len, uint8_t *addr_out, int reverse);

#endif /* NANO_IEEE802154_H */
