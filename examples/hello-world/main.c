/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Hello World application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "wq.h"
#include "ztimer.h"
#include "thread_flags.h"

void *thread(void *arg)
{
    wq_t *wq = arg;

    printf("thread started, pid: %" PRIkernel_pid "\n", thread_getpid());

    wq_waiter_t waiter;
    wq_attach(wq, &waiter);

    while (1) {
        thread_flags_t flags = thread_flags_wait_any(0x01);
        if (flags & 1) {
            printf("thread woke up, pid: %" PRIkernel_pid "\n", thread_getpid());
            ///
        }
    }

    return NULL;
}

char second_thread_stack[THREAD_STACKSIZE_MAIN];
char third_thread_stack[THREAD_STACKSIZE_MAIN];

int main(void)
{
    puts("Hello World!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s CPU.\n", RIOT_CPU);

    wq_t wq = {.waiters.next = NULL };

    thread_create(second_thread_stack, sizeof(second_thread_stack),
                            THREAD_PRIORITY_MAIN - 1, 0,
                            thread, &wq, "pong");


    thread_create(third_thread_stack, sizeof(third_thread_stack),
                            THREAD_PRIORITY_MAIN - 1, 0,
                            thread, &wq, "pong");

    while(1) {
        printf("waking... \n");
        wq_wake_all(&wq);

        printf("sleeping... \n");
        ztimer_sleep(ZTIMER_MSEC, 1000);
    }

    return 0;
}
