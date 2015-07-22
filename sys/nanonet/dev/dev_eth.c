
/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup nanonet
 * @{
 *
 * @file
 * @brief       nanonet ethernet driver glue
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <errno.h>
#include <string.h>

#include "byteorder.h"
#include "kernel_types.h"
#include "thread.h"

#include "net/dev_eth.h"
#include "dev_eth_autoinit.h"
#include "nanonet.h"
#include "nano_sndbuf.h"

#define ENABLE_DEBUG ENABLE_NANONET_DEBUG
#include "debug.h"

extern kernel_pid_t nanonet_pid;

#if NUM_DEV_ETH > 1
#warning "nanonet: Support for more than one dev_eth is untested!"
#endif

static inline int dev_eth_index(dev_eth_t *dev) {
    if (NUM_DEV_ETH > 1) {
        return (dev - dev_eth_devices[0]) / sizeof(dev_eth_devices[0]);
    } else {
        return 0;
    }
}

static inline nano_dev_t* eth_to_nano(dev_eth_t *dev) {
    return &nanonet_devices[NANO_DEV_ETH + dev_eth_index(dev)];
}

/* dev_eth handler implementations */
void dev_eth_isr(dev_eth_t *dev)
{
    nanonet_iflags |= (1<<(dev_eth_index(dev) + NANO_DEV_ETH));
    mutex_unlock(&nanonet_mutex);
}

void dev_eth_rx_handler(dev_eth_t *dev)
{
    /* read packet from device into nanonet's global rx buffer */
    int nbytes = dev->driver->recv(dev, (char*)nanonet_rxbuf, NANONET_RX_BUFSIZE);

/*    DEBUG("dev_eth_receive_packet_cb(): received packet with len %u, ethertype 0x%04x\n",
            nbytes, (unsigned int) NTOHS(((eth_hdr_t*) nanonet_rxbuf)->ethertype));
*/
    nano_eth_handle(eth_to_nano(dev), nanonet_rxbuf, nbytes);
}

/* nanonet nano_dev_t implementations */
static int send(nano_dev_t *dev, nano_sndbuf_t *buf, uint8_t* dest_mac, uint16_t ethertype) {
    DEBUG("nanonet_dev_eth_send: Sending packet with len %u\n", buf->used);
    dev_eth_t *ethdev = (dev_eth_t *) dev->ptr;
    eth_hdr_t *hdr = (eth_hdr_t *) nano_sndbuf_alloc(buf, sizeof(eth_hdr_t));

    if (!hdr) {
        DEBUG("nanonet_dev_eth_send: buffer too small.\n");
        return -ENOSPC;
    }

    memcpy(hdr->dst, dest_mac, 6);
    memcpy(hdr->src, dev->mac_addr, 6);

    hdr->ethertype = HTONS(ethertype);

    ethdev->driver->send(ethdev, (char*)hdr, buf->used);

    return 0;
}

static int send_raw(nano_dev_t *dev, uint8_t* buf, size_t len) {
    DEBUG("nanonet_dev_eth_send_raw: Sending packet with len %u\n", len);
    dev_eth_t *ethdev = (dev_eth_t *) dev->ptr;
    ethdev->driver->send(ethdev, (char*)buf, len);

    return 0;
}

static int l2_needed(nano_dev_t *dev)
{
    (void)dev;

    return sizeof(eth_hdr_t);
}

int nanonet_init_dev_eth(int n)
{
    DEBUG("nanonet_init_dev_eth\n");
    dev_eth_t *dev = dev_eth_devices[n - NANO_DEV_ETH];
    nano_dev_t *nanodev = &nanonet_devices[n];

    nanodev->send = send;
    nanodev->send_raw = send_raw;
    nanodev->l2_needed = l2_needed;
    nanodev->ptr = (void*)dev;
    nanodev->handle_isr = (void (*)(void*))dev->driver->isr;
    nanodev->reply = nano_eth_reply;

    dev_eth_init(dev);

    dev->driver->get_mac_addr(dev, (uint8_t*)nanodev->mac_addr);

    /* set link-local address */
    memset(nanodev->ipv6_ll, 0, IPV6_ADDR_LEN);
    nanodev->ipv6_ll[0] = 0xfe;
    nanodev->ipv6_ll[1] = 0x80;
    nano_eth_get_iid((nanodev->ipv6_ll + 8), nanodev->mac_addr);

#if ENABLE_DEBUG
    printf("nanonet_init_dev_eth: Setting link-layer address ");
    ipv6_addr_print(nanodev->ipv6_ll);
    puts("");
#endif

    nanonet_iflags |= 0x1<<n;

    return 1;
}

