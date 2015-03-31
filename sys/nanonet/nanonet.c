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

#include "atomic.h"
#include "board.h"
#include "byteorder.h"
#include "hwtimer.h"
#include "thread.h"
#include "vtimer.h"

#include "periph/gpio.h"
#include "periph/spi.h"

#include "net/dev_eth.h"

#include "nanonet.h"

#define ENABLE_DEBUG ENABLE_NANONET_DEBUG
#include "debug.h"

kernel_pid_t nanonet_pid = KERNEL_PID_UNDEF;
volatile unsigned nanonet_iflags;
mutex_t nanonet_mutex = MUTEX_INIT;
char nanonet_rxbuf[NANONET_RX_BUFSIZE];

void nanonet_init(void)
{
    DEBUG("nanonet: initializing...\n");

    nanonet_init_devices();

    DEBUG("nanonet: initialization complete.\n");
}

/*static inline unsigned atomic_waitnonzero(unsigned int *val, mutex_t *mutex) {
    unsigned state = disableIRQ();

    unsigned tmp = *val;
    *val = 0;

    if(!tmp) {
        restoreIRQ(state);
    }

    return tmp;
}*/

void nanonet_loop(void)
{
    nanonet_pid = thread_getpid();

    DEBUG("nanonet event loop started.\n");

    nano_dev_t *dev;
    unsigned int flags;

    while(1) {
        if ((flags = (unsigned)atomic_set_return((atomic_int_t*)&nanonet_iflags, 0))) {
            for (int i = 0; i < NANO_NUM_DEVS; i++) {
                if (flags & (0x1 << i)) {
                    DEBUG("nanonet: shooting handler %i\n", i);
                    dev = (nano_dev_t*) &nanonet_devices[i];
                    dev->handle_isr(dev->ptr);
                    DEBUG("nanonet: return from handler\n");
                }
            }
        } else {
            mutex_lock(&nanonet_mutex);
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
