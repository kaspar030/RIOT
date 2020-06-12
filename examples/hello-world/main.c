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

#include "ztimer.h"

int main(void)
{
    puts("Hello World!");

    for (unsigned i = 0; i < 10; i++) {
        uint32_t before = ztimer_now(ZTIMER_USEC);
        ztimer_now(ZTIMER_MSEC);
        printf("%"PRIu32"\n", ztimer_now(ZTIMER_USEC) - before);
    }

    puts("Test successful.");

    return 0;
}
