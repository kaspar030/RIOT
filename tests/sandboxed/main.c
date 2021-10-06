/*
 * Copyright (C) 2021 Inria
 *               2021 Freie Universität Berlin
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
 * @brief       testing sandbox
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */
#include "thread.h"

static thread_t sandboxed;
static char stack[THREAD_STACKSIZE_MAIN];

static void *_sandboxed(void *arg)
{
    (void)arg;

    puts("_sandboxed()");
    return 0;
}

int main(void)
{
    kernel_pid_t pid = thread_create_detached(&sandboxed, stack, sizeof(stack),
                                              THREAD_PRIORITY_MAIN,
                                              THREAD_CREATE_STACKTEST,
                                              _sandboxed, NULL, "sandboxed");

    printf("pid=%" PRIkernel_pid "\n", pid);
    return 0;
}
