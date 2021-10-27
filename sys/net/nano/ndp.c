/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_nanonet
 * @{
 *
 * @file
 * @brief       Nanonet's NDP implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>

#include "net/nano/config.h"
#include "net/nano/ipv6.h"
#include "net/nano/ndp.h"
#include "net/nano/util.h"

#define ENABLE_DEBUG    ENABLE_NANONET_DEBUG
#include "debug.h"

#ifndef NANO_NDP_CACHE_SIZE
#define NANO_NDP_CACHE_SIZE         16
#endif

#define MAX_L2_ADDR_LEN             6

#define NANO_NDP_STATE_INCOMPLETE   (0x01)
#define NANO_NDP_STATE_REACHABLE    (0x02)
#define NANO_NDP_STATE_STALE        (0x03)
#define NANO_NDP_STATE_DELAY        (0x04)
#define NANO_NDP_STATE_PROBE        (0x05)

typedef struct {
    uint8_t flags;
    uint8_t l3_addr[IPV6_ADDR_LEN];
    size_t l2_addr_len;
    uint8_t l2_addr[MAX_L2_ADDR_LEN];
} cache_entry_t;

static cache_entry_t cache[NANO_NDP_CACHE_SIZE];

void nano_ndp_init(void)
{
    for (int i = 0; i < NANO_NDP_CACHE_SIZE; i++) {
        cache[i].flags = 0;
    }
}

size_t nano_ndp_lookup(const nano_dev_t *dev, uint8_t *l3_addr, uint8_t **l2_addr)
{
    (void)dev;
    (void)l3_addr;
    (void)l2_addr;
    return 0;
}

int nano_ndp_update(nano_ctx_t *ctx)
{
    (void)ctx;
    return 0;
}

int nano_ndp_add(uint8_t *l3_addr, uint8_t *l2_addr, size_t l2_addr_len)
{
    int i = 0;
    while (i < NANO_NDP_CACHE_SIZE && !(cache[i].flags)) {
        i++;
    }
    if (i == NANO_NDP_CACHE_SIZE) {
        DEBUG("[nano ndp] error adding entry: chache full\n");
        return -1;
    }
    cache[i].flags = NANO_NDP_STATE_REACHABLE;
    memcpy(&(cache[i].l3_addr), l3_addr, IPV6_ADDR_LEN);
    memcpy(&(cache[i].l2_addr), l2_addr, l2_addr_len);
    return 0;
}

int nano_ndp_sync(void)
{
    for (int i = 0; i < NANO_NDP_CACHE_SIZE; i++){
        if (cache[i].flags){
            /* TODO do something with this entry */
        }
    }
    return 0;
}

#if 0
void nano_ndp_dump(void)
{
    puts("IPv6 neighbor cache\n");
    puts("L3 ADDR     L2 ADDR     L2 ADDR LEN     IFACE");
    puts("---------------------------------------------");
    for (int i = 0; i < NANO_NDP_CACHE_SIZE; i++) {
        if (cache[i].flags) {
            // nano_ipv6_addr_dump(&(cache[i].l3_addr));
            printf("   ");
            nano_util_addr_dump(cache[i].l2_addr, cache[i].l2_addr_len);
            printf("   %u\n", (unsigned)cache[i].l2_addr_len);
        }
    }
}
#endif
