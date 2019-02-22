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

#include "ps.h"


int main(void)
{
    puts("Hello World!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    puts("");
    printf("\"%s\"\n", "123");
    printf("\"%5s\"\n", "123");
    printf("\"%-5s\"\n", "123");

    puts("");
    printf("\"%u\"\n", 123);
    printf("\"%5u\"\n", 123);
    printf("\"%-5u\"\n", 123);
    printf("\"%u\"\n", 0xffffffff);
    printf("\"%u\"\n", sizeof(unsigned int));

    puts("");
    printf("\"%d\"\n", 123);
    printf("\"%5d\"\n", 123);
    printf("\"%-5d\"\n", 123);
    printf("\"%d\"\n", 0xffffffff);

    puts("");
    printf("\"%d\"\n", -123);
    printf("\"%5d\"\n", -123);
    printf("\"%-5d\"\n", -123);

    puts("");
/*    char dummy;
    printf("\"%p\"\n", (void *)&dummy);
    printf("\"%12p\"\n", (void *)&dummy);
    printf("\"%-12p\"\n", (void *)&dummy);
*/
    ps();
    ps();
    return 0;
}
