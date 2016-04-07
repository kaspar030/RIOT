#include <string.h>

#include "byteorder.h"

#include "nanonet.h"

#define ENABLE_DEBUG ENABLE_NANONET_DEBUG
#include "debug.h"

void nano_ieee802154_handle(nano_dev_t *dev, uint8_t *buf, size_t len)
{
    (void)dev;
    (void)buf;
    (void)len;
    /* setup crosslayer context struct */
//    nano_ctx_t ctx = { .dev=dev, .buf=buf, .len=len};
    DEBUG("nano_ieee802154_handle()\n");
}

int nano_ieee802154_reply(nano_ctx_t *ctx)
{
    (void)ctx;
    DEBUG("nano_ieee802154_reply()\n");
    return 0;
}
