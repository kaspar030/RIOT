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
 * @brief       nanonet network stack example application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "xtimer.h"

#include "net/nano.h"
#include "net/nano/ipv4.h"
#include "net/nano/ipv6.h"
#include "net/nano/route.h"
#include "net/nano/udp.h"
#include "nano_coap.h"

#ifdef MODULE_NANONET_TCP
#include "net/nano/tcp.h"
#endif

#include "log.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

int nano_dummy_handler(nano_ctx_t *ctx, size_t offset, ...) {
    DEBUG("udp: packet received\n");
    (void)ctx;
    (void)offset;

    return 0;
}

#ifdef MODULE_NANONET_IPV4
ipv4_route_t ipv4_routes[] = {
    { IP4(192,168,111,0), IP4(255,255,255,0), 0, &nanonet_devices[0] }, /* 192.168.111.0/24 for dev 0 */
    {0}
};
#endif /* MODULE_NANONET_IPV4 */

ipv6_route_t ipv6_routes[] = {
    { {0}, 0, {0}, 0}
};

nano_udp_bind_t _coap_bind = { .port=5683, .handler=nano_coap_handler };
nano_udp_bind_t _dummy_bind = { .port=12345, .handler=nano_dummy_handler };

#ifdef MODULE_NANONET_TCP
static uint8_t _tcp_buf[256];
static char _tcp_stack[THREAD_STACKSIZE_MAIN];
static tcp_tcb_t tcb;
static void *_tcp_thread(void *arg)
{
    (void)arg;
    /* send dummy packet to populate arp cache */
    iolist_t tmp = {0};
    xtimer_usleep(1000000LU);
    /* send udp packet to fill ARP cache */
    udp_send(&tmp, IP4(192,168,111,1), 12345, 12345);
    xtimer_usleep(1000000LU);

    tcp_init(&tcb, _tcp_buf, sizeof(_tcp_buf));
    tcp_connect(&tcb, IP4(192,168,111,1), 12345, 12345);

    xtimer_sleep(1);
    tmp.iol_base = "foo\n";
    tmp.iol_len = 4;
    while (tcb.state == ESTABLISHED) {
        puts("tcp sending...");
        tcp_write(&tcb, &tmp);
        xtimer_sleep(1);
    }

    return NULL;
}
#endif

int main(void)
{
    LOG_INFO("nanonet test app started.\n");

    /* initialize network stack and devices */
    nanonet_init();

    clist_rpush(&nano_udp_binds, &_coap_bind.next);
    clist_rpush(&nano_udp_binds, &_dummy_bind.next);

    /* set IP */
#ifdef MODULE_NANONET_IPV4
    nanonet_devices[0].ipv4 = IP4(192,168,111,2);
#endif /* MODULE_NANONET_IPV4 */

#ifdef MODULE_NANONET_TCP
    thread_create(_tcp_stack, sizeof(_tcp_stack),
                  THREAD_PRIORITY_MAIN + 1,
                  THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                  _tcp_thread, NULL, "_tcp");
#endif

    /* start nanonet event loop, will not return */
    nanonet_loop();

    return 0;
}
