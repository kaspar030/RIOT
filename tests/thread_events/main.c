/*
 * Copyright (C) 2023 Freie Universität Berlin
 *               2023 Inria
 *               2023 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       Test for thread event semantics
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdatomic.h>
#include <stdio.h>

#include "thread.h"
#include "thread_events.h"

#include "test_utils/expect.h"

static char _stack[THREAD_STACKSIZE_MAIN];

static thread_event_t _events[16];

static void *_second_thread(void *arg)
{
    (void)arg;

    expect(thread_event_get() == &_events[0]);

    expect(thread_event_get_this(&_events[2]) == &_events[2]);

    expect(thread_event_get() == &_events[1]);
    expect(thread_event_get() == &_events[3]);

    expect(thread_event_get_oneof(&_events[7], 2) == &_events[7]);
    expect(thread_event_get_oneof(&_events[7], 2) == &_events[8]);
    expect(thread_event_get() == &_events[4]);
    expect(thread_event_get() == &_events[5]);
    expect(thread_event_get() == &_events[6]);
    expect(thread_event_get() == &_events[9]);

    printf("[SUCCESS]\n");

    return NULL;
}

int main(void)
{
    puts("main starting");

    kernel_pid_t other_pid = thread_create(_stack,
                                       sizeof(_stack),
                                       (THREAD_PRIORITY_MAIN - 1),
                                       THREAD_CREATE_STACKTEST,
                                       _second_thread,
                                       NULL,
                                       "second_thread");

    thread_t *thread = thread_get(other_pid);

    for (unsigned i = 0; i < sizeof(_events); i++) {
        thread_event_post(thread, &_events[i]);
    }

    return 0;
}
