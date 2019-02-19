#ifndef NANO_ARP_H
#define NANO_ARP_H

#include <stdint.h>
#include "nano_ctx.h"
#include "nano_dev.h"

int arp_handle(nano_ctx_t *ctx, size_t offset);

int arp_cache_get(nano_dev_t *dev, uint32_t dest_ip, uint8_t *mac_addr_out);
void arp_cache_update(nano_dev_t *dev, uint32_t dest_ip, uint8_t *mac);

#endif /* NANO_ARP_H */
