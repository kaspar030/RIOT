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

#define ENABLE_DEBUG 1
#include "debug.h"

ipv4_route_t ipv4_routes[] = {
    { 0xc0a86f00, 0xffffff00, &nanonet_devices[0] }, /* 192.168.111.0/24 for dev 0 */
    {0}
};

nano_udp_bind_t nano_udp_binds[] = {
    { 5683, nano_coap_handler },
    {0}
};

int main(void)
{
    printf("nanonet test app started.\n");

    /* initialize network stack and devices */
    nanonet_init();

    /* set IP */
    nanonet_devices[0].ipv4 = 0xc0a86f02; /* 0xc0a86f02 = 192.168.111.2 */

    /* start nanonet event loop, will not return */
    nanonet_loop();

    return 0;
}
