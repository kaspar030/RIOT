/*
 * Copyright (C) 2023 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Measure messages send per second for mbox_sync
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdatomic.h>
#include <stdio.h>
#include "macros/units.h"
#include "thread.h"
#include "clk.h"

#include "mq.h"
#include "xtimer.h"

#ifndef TEST_DURATION_US
#define TEST_DURATION_US    (1000000U)
#endif

static char _stack[THREAD_STACKSIZE_MAIN];

static void _timer_callback(void *flag)
{
    atomic_flag_clear(flag);
}

static void *_second_thread(void *arg)
{
    mq_t *mq = arg;

    while (1) {
        mq_msg_t test;
        mq_rx(mq, &test);
    }

    return NULL;
}

static mq_t mq = { 0 };

int main(void)
{
    puts("main starting");

    thread_create(_stack, sizeof(_stack), (THREAD_PRIORITY_MAIN - 1),
                  THREAD_CREATE_STACKTEST, _second_thread, &mq,
                  "second_thread");

    atomic_flag flag = ATOMIC_FLAG_INIT;
    uint32_t n = 0;

    xtimer_t timer = {
        .callback = _timer_callback,
        .arg = &flag,
    };

    atomic_flag_test_and_set(&flag);
    xtimer_set(&timer, TEST_DURATION_US);

    while (atomic_flag_test_and_set(&flag)) {
        mq_msg_t test;
        mq_tx(&mq, &test);
        n++;
    }

    printf("{ \"result\" : %" PRIu32, n);
    printf(", \"ticks\" : %" PRIu32,
           (uint32_t)((TEST_DURATION_US / US_PER_MS) * (coreclk() / KHZ(1))) /
           n);
    puts(" }");

    return 0;
}
