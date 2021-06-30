/*
 * Copyright (C) 2021 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       ztimer64 test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "ztimer.h"
#include "ztimer64.h"

void callback(void *arg) {
    (void)arg;
    puts("callback");
}

int main(void)
{
    ztimer64_clock_t msec;
    ztimer64_clock_init(&msec, ZTIMER_MSEC);
    ztimer64_t timer = { .callback=callback };
    ztimer64_set(&msec, &timer, 2000);

    while(1) {
        printf("now64=%llu\n", ztimer64_now(&msec));
        printf("now32=%u\n", ztimer_now(ZTIMER_MSEC));
        ztimer_sleep(ZTIMER_MSEC, 1000);
    }
    return 0;
}
