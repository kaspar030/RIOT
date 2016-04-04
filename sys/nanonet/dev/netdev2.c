
/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       nanonet netdev2 driver glue
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
#include "thread_flags.h"

#include "sys/uio.h"

#include "net/netdev2/eth.h"
#include "nanonet.h"
#include "nano_sndbuf.h"

#define ENABLE_DEBUG ENABLE_NANONET_DEBUG
#include "debug.h"

#ifdef MODULE_NETDEV2_TAP
#include "netdev2_tap.h"
#include "net/gnrc/netdev2/eth.h"
extern netdev2_tap_t netdev2_tap;
#endif

#ifdef MODULE_ETHOS
#include "ethos.h"
ethos_t ethos;
static uint8_t _ethos_inbuf[2048];
#endif

static void _netdev2_isr(netdev2_t *netdev, netdev2_event_t event, void* arg)
{
    if (event == NETDEV2_EVENT_ISR) {
        unsigned n = (unsigned)netdev->isr_arg;
        thread_flags_set(nanonet_thread, 0x1<<n);
        return;
    }

    nano_dev_t *dev = &nanonet_devices[(unsigned)netdev->isr_arg];

    switch(event) {
        case NETDEV2_EVENT_RX_COMPLETE:
            {
                /* read packet from device into nanonet's global rx buffer */
                int nbytes = netdev->driver->recv(netdev, (char*)nanonet_rxbuf,
                        NANONET_RX_BUFSIZE, NULL);
                if (nbytes > 0) {
                    nano_eth_handle(dev, nanonet_rxbuf, nbytes);
                }
                else {
                    DEBUG("_netdev2_isr(): recv <= 0\n");
                }

            }
            break;
        default:
            printf("_netdev2_isr(): unhandled event: %u\n", (unsigned)event);
    }
}

static int _send_ethernet(nano_dev_t *dev, nano_sndbuf_t *buf, uint8_t* dest_mac, uint16_t ethertype)
{
    DEBUG("nanonet_netdev2_send: Sending packet with len %u\n", buf->used);
    netdev2_t *netdev = (netdev2_t *) dev->netdev;
    eth_hdr_t *hdr = (eth_hdr_t *) nano_sndbuf_alloc(buf, sizeof(eth_hdr_t));

    if (!hdr) {
        DEBUG("_send_ethernet: buffer too small.\n");
        return -ENOSPC;
    }

    memcpy(hdr->dst, dest_mac, 6);
    memcpy(hdr->src, dev->l2_addr, 6);

    hdr->ethertype = HTONS(ethertype);

    struct iovec vec = { hdr, buf->used };
    netdev->driver->send(netdev, &vec, 1);

    return 0;
}

static int _send_raw(nano_dev_t *dev, uint8_t* buf, size_t len)
{
    DEBUG("nanonet_netdev2_send_raw: Sending packet with len %u\n", len);
    netdev2_t *netdev = (netdev2_t *) dev->netdev;
    struct iovec vec = { buf, len };
    netdev->driver->send(netdev, &vec, 1);

    return 0;
}

int nanonet_init_netdev2_eth(unsigned devnum, netdev2_t *netdev)
{
    DEBUG("nanonet_init_netdev2\n");
    nano_dev_t *nanodev = &nanonet_devices[devnum];

    nanodev->send = _send_ethernet;
    nanodev->send_raw = _send_raw;
    nanodev->l2_needed = sizeof(eth_hdr_t);
    nanodev->netdev = (void*)netdev;
    nanodev->reply = nano_eth_reply;
    nanodev->handle_isr = NULL; //_recv_ethernet;

    /* initialize low-level driver */
    netdev->driver->init(netdev);

    /* register the event callback with the device driver */
    netdev->event_callback = _netdev2_isr;
    netdev->isr_arg = (void*) devnum;

    /* set up addresses */
    netdev->driver->get(netdev, NETOPT_ADDRESS, (uint8_t*)nanodev->l2_addr, 6);

    memset(nanodev->ipv6_ll, 0, IPV6_ADDR_LEN);
    nanodev->ipv6_ll[0] = 0xfe;
    nanodev->ipv6_ll[1] = 0x80;
    nano_eth_get_iid((nanodev->ipv6_ll + 8), nanodev->l2_addr);

#if ENABLE_DEBUG
    puts("nanonet_init_dev_eth: Setting link-layer address ");
    ipv6_addr_print(nanodev->ipv6_ll);
    puts("");
#endif

    return 1;
}

static const netdev2_t *_netdevs[] = {
#ifdef MODULE_NETDEV2_TAP
    (netdev2_t*)&netdev2_tap,
#endif
#ifdef MODULE_ETHOS
    (netdev2_t*)&ethos,
#endif
};

const unsigned nano_dev_numof = sizeof(_netdevs)/sizeof(netdev2_t *);

nano_dev_t nanonet_devices[sizeof(_netdevs)/sizeof(netdev2_t *)];

void nanonet_init_devices(void)
{
#ifdef MODULE_ETHOS
    const ethos_params_t ethos_params = {
        .uart=ETHOS_UART,
        .baudrate=ETHOS_BAUDRATE,
        .buf=_ethos_inbuf,
        .bufsize=sizeof(_ethos_inbuf) };

    ethos_setup(&ethos, &ethos_params);
#endif

    for (unsigned n = 0; n < nano_dev_numof; n++) {
        nanonet_init_netdev2_eth(n, (netdev2_t*)_netdevs[n]);
    }
}
