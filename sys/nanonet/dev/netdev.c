
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
 * @brief       nanonet netdev driver glue
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
#include "xtimer.h"
#include "sys/uio.h"

#include "net/netdev/eth.h"
//#include "net/netdev/ieee802154.h"

#include "nanonet.h"

#define ENABLE_DEBUG ENABLE_NANONET_DEBUG
#include "debug.h"

#ifdef MODULE_NETDEV_TAP
#include "netdev_tap.h"
#include "netdev_tap_params.h"
netdev_tap_t netdev_tap;
#endif

#ifdef MODULE_ETHOS
#include "ethos.h"
ethos_t ethos;
static uint8_t _ethos_inbuf[2048];
#endif

#ifdef MODULE_AT86RF2XX
#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
static at86rf2xx_t at86rf2xx;
#endif

#ifdef MODULE_ENCX24J600
#include "encx24j600.h"
static encx24j600_t encx24j600;
#endif

static const netdev_t *_netdevs[] = {
#ifdef MODULE_NETDEV_TAP
    (netdev_t*)&netdev_tap,
#endif
#ifdef MODULE_ETHOS
    (netdev_t*)&ethos,
#endif
#ifdef MODULE_AT86RF2XX
    (netdev_t*)&at86rf2xx,
#endif
#ifdef MODULE_ENCX24J600
    (netdev_t*)&encx24j600,
#endif
};

const unsigned nano_dev_numof = sizeof(_netdevs)/sizeof(netdev_t *);

nano_dev_t nanonet_devices[sizeof(_netdevs)/sizeof(netdev_t *)];

static void _netdev_isr(netdev_t *netdev, netdev_event_t event)
{
    if (event == NETDEV_EVENT_ISR) {
        unsigned n = (unsigned)netdev->context;
        thread_flags_set(nanonet_thread, 0x1<<n);
        return;
    }

    nano_dev_t *dev = &nanonet_devices[(unsigned)netdev->context];

    switch(event) {
        case NETDEV_EVENT_RX_COMPLETE:
            {
                int nbytes;
                do {
                    /* read packet from device into nanonet's global rx buffer */
                    nbytes = netdev->driver->recv(netdev, ((char*)nanonet_rxbuf),
                            NANONET_RX_BUFSIZE, NULL);
                    if (nbytes > 0) {
                        dev->handle_rx(dev, nanonet_rxbuf, nbytes);
                    }
                } while (nbytes != -1);
            }
            break;
        case NETDEV_EVENT_TX_COMPLETE:
            break;
        default:
            DEBUG("_netdev_isr(): unhandled event: %u\n", (unsigned)event);
    }
}

static int _send_ethernet(nano_dev_t *dev, const iolist_t *iolist, uint8_t* dest_mac, uint16_t ethertype)
{
    DEBUG("nanonet_netdev_send: Sending packet with len %u\n", (unsigned)iolist_size(iolist));
    netdev_t *netdev = (netdev_t *) dev->netdev;
    eth_hdr_t hdr = { .ethertype = htons(ethertype) };

    memcpy(hdr.dst, dest_mac, 6);
    memcpy(hdr.src, dev->l2_addr, 6);

    iolist_t _iolist = { (iolist_t *)iolist, &hdr, sizeof(hdr) };
    netdev->driver->send(netdev, &_iolist);

    return 0;
}

static int _send_raw(nano_dev_t *dev, uint8_t *buf, size_t len)
{
    DEBUG("nanonet_netdev_send_raw: Sending packet with len %u\n", len);
    netdev_t *netdev = (netdev_t *) dev->netdev;
    iolist_t iolist = { NULL, buf, len };
    netdev->driver->send(netdev, &iolist);
    return 0;
}

#ifdef NANONET_IEEE802154
static int _send_ieee80154(nano_dev_t *dev, const iolist_t *iolist, uint8_t* dest_l2addr)
{
    (void)dev;
    (void)dest_l2addr;
    DEBUG("_send_ieee80154(): Sending packet with len %u\n", (unsigned)iolist_size(iolist));
    return 0;
}

int nanonet_init_netdev_ieee802154(unsigned devnum)
{
    DEBUG("nanonet_init_netdev\n");
    nano_dev_t *nanodev = &nanonet_devices[devnum];
    netdev_t *netdev = (netdev_t*)_netdevs[devnum];

    nanodev->send = (nano_dev_send_t) _send_ieee80154;
    nanodev->send_raw = _send_raw;
    nanodev->l2_needed = sizeof(eth_hdr_t);
    nanodev->netdev = (void*)netdev;
    nanodev->reply = nano_ieee802154_reply;
    nanodev->handle_rx = nano_ieee802154_handle;

    /* initialize low-level driver */
    netdev->driver->init(netdev);

    /* register the event callback with the device driver */
    netdev->event_callback = _netdev_isr;
    netdev->context = (void*) devnum;

    /* set up addresses */
    netdev->driver->get(netdev, NETOPT_ADDRESS_LONG, (uint8_t*)nanodev->l2_addr, 8);

    memset(nanodev->ipv6_ll, 0, IPV6_ADDR_LEN);
    nanodev->ipv6_ll[0] = 0xfe;
    nanodev->ipv6_ll[1] = 0x80;
    nano_ieee802154_get_iid(nanodev->l2_addr, 8, (nanodev->ipv6_ll + 8), 0);

#if ENABLE_DEBUG
    puts("nanonet_init_netdev: Setting link-layer address ");
    ipv6_addr_print(nanodev->ipv6_ll);
    puts("");
#endif

    return 1;
}
#endif

int nanonet_init_netdev_eth(unsigned devnum)
{
    DEBUG("nanonet_init_netdev\n");
    nano_dev_t *nanodev = &nanonet_devices[devnum];
    netdev_t *netdev = (netdev_t*)_netdevs[devnum];

    nanodev->send = (nano_dev_send_t) _send_ethernet;
    nanodev->send_raw = _send_raw;
    nanodev->l2_needed = sizeof(eth_hdr_t);
    nanodev->netdev = (void*)netdev;
    nanodev->reply = nano_eth_reply;
    nanodev->handle_rx = nano_eth_handle;

    /* register the event callback with the device driver */
    netdev->event_callback = _netdev_isr;
    netdev->context = (void*) devnum;

    /* initialize low-level driver */
    netdev->driver->init(netdev);

    /* set up addresses */
    netdev->driver->get(netdev, NETOPT_ADDRESS, (uint8_t*)nanodev->l2_addr, 6);

    memset(nanodev->ipv6_ll, 0, IPV6_ADDR_LEN);
    nanodev->ipv6_ll[0] = 0xfe;
    nanodev->ipv6_ll[1] = 0x80;
    nano_eth_get_iid((nanodev->ipv6_ll + 8), nanodev->l2_addr);

#if ENABLE_DEBUG
    printf("nanonet_init_dev_eth: Setting link-layer address ");
    ipv6_addr_print(nanodev->ipv6_ll);
    puts("");
#endif

    return 1;
}

void nanonet_init_devices(void)
{
    unsigned n = 0;
#ifdef MODULE_NETDEV_TAP
    netdev_tap_setup(&netdev_tap, &netdev_tap_params[0]);
    nanonet_init_netdev_eth(n++);
#endif

#ifdef MODULE_ETHOS
    const ethos_params_t ethos_params = {
        .uart=ETHOS_UART,
        .baudrate=ETHOS_BAUDRATE,
        .buf=_ethos_inbuf,
        .bufsize=sizeof(_ethos_inbuf) };

    ethos_setup(&ethos, &ethos_params);
    nanonet_init_netdev_eth(n++);
#endif

#ifdef MODULE_AT86RF2XX
    at86rf2xx_setup(&at86rf2xx, (at86rf2xx_params_t*) &at86rf2xx_params[0]);
    nanonet_init_netdev_ieee802154(n++);
#endif

#ifdef MODULE_ENCX24J600
    encx24j600_params_t encx24j600_params = {
        .spi       = ENCX24J600_SPI,
        .cs_pin    = ENCX24J600_CS,
        .int_pin   = ENCX24J600_INT
    };
    encx24j600_setup(&encx24j600, &encx24j600_params);
    nanonet_init_netdev_eth(n++);
#endif
}
