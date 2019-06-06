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
 * @brief       nanonet network stack glue code
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "bitarithm.h"
#include "board.h"
#include "byteorder.h"
#include "event.h"
#include "thread.h"
#include "thread_flags.h"

#include "periph/gpio.h"
#include "periph/spi.h"

#include "nanonet.h"

#define ENABLE_DEBUG ENABLE_NANONET_DEBUG
#include "debug.h"

thread_t *nanonet_thread;
uint8_t nanonet_rxbuf[NANONET_RX_BUFSIZE];

void nanonet_init(void)
{
    DEBUG("nanonet: initializing...\n");

    nanonet_thread = (thread_t *)sched_active_thread;

    nanonet_init_devices();

    DEBUG("nanonet: initialization complete.\n");
}

void nanonet_loop(void)
{
    DEBUG("nanonet event loop started.\n");

    nano_dev_t *dev;
    thread_flags_t flag;

    while(1) {
        flag = thread_flags_wait_one(0xffff);
        if (flag & THREAD_FLAG_EVENT) {
        }
        else {
            unsigned netdev_num = bitarithm_lsb(flag >> 1);
            assert(netdev_num < nano_dev_numof);
            dev = (nano_dev_t*) &nanonet_devices[netdev_num];
            dev->netdev->driver->isr(dev->netdev);
        }
    }
}

/* optional. nanonet() can be run from whithin main. */
#if 0
static char nanonet_stack[KERNEL_CONF_STACKSIZE_MAIN];
static void* nanonet_thread(void* arg) {
    (void)arg;
    nanonet_loop();
    return NULL;
}

void nanonet_start_thread(void) {
    thread_create(nanonet_stack, sizeof(nanonet_stack), PRIORITY_MAIN -1,
            CREATE_STACKTEST,
            nanonet_thread, NULL, "nanonet");
}
#endif
