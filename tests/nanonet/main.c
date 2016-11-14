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

int nano_dummy_handler(nano_ctx_t *ctx, size_t offset) {
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

nano_udp_bind_t nano_udp_binds[] = {
    { 5683, nano_coap_handler },
    { 12345, nano_dummy_handler },
    {0}
};

int main(void)
{
    LOG_INFO("nanonet test app started.\n");

    /* initialize network stack and devices */
    nanonet_init();

    /* set IP */
#ifdef NANONET_IPV4
    nanonet_devices[0].ipv4 = 0xc0a86f02; /* 0xc0a86f02 = 192.168.111.2 */
#endif /* NANONET_IPV4 */

    /* start nanonet event loop, will not return */
    nanonet_loop();

    return 0;
}
