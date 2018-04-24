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

#include "nanonet.h"
#include "nano_coap.h"

#include "log.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

int nano_dummy_handler(nano_ctx_t *ctx, size_t offset, ...) {
    DEBUG("udp: packet received\n");
    (void)ctx;
    (void)offset;

    return 0;
}

#ifdef NANONET_IPV4
ipv4_route_t ipv4_routes[] = {
    { 0xc0a86f00, 0xffffff00, 0, &nanonet_devices[0] }, /* 192.168.111.0/24 for dev 0 */
    {0}
};
#endif /* NANONET_IPV4 */

ipv6_route_t ipv6_routes[] = {
    { {0}, 0, {0}, 0}
};

nano_udp_bind_t _coap_bind = { .port=5683, .handler=nano_coap_handler };
nano_udp_bind_t _dummy_bind = { .port=12345, .handler=nano_dummy_handler };

int main(void)
{
    LOG_INFO("nanonet test app started.\n");

    /* initialize network stack and devices */
    nanonet_init();

    clist_rpush(&nano_udp_binds, &_coap_bind.next);
    clist_rpush(&nano_udp_binds, &_dummy_bind.next);

    /* set IP */
#ifdef NANONET_IPV4
    nanonet_devices[0].ipv4 = 0xc0a86f02; /* 0xc0a86f02 = 192.168.111.2 */
#endif /* NANONET_IPV4 */

    /* start nanonet event loop, will not return */
    nanonet_loop();

    return 0;
}
