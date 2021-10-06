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
#include <stdio.h>

#include "cpu.h"
#include "thread.h"
#include "sandbox.h"

static thread_t sandboxed;
static sandbox_t sandbox;

static char mem[THREAD_STACKSIZE_MAIN * 2];

bool is_privileged(void)
{
    return (__get_IPSR()>0) || ((__get_CONTROL() & CONTROL_nPRIV_Msk) == 0);
}

static void *_sandboxed(void *arg)
{
    (void)arg;

    puts("_sandboxed()");
    if (is_privileged()) {
        puts("priviledged");
    }
    else {
        puts("unpriviledged");
    }
    return 0;
}

kernel_pid_t thread_create_sandboxed(thread_t *thread, sandbox_t *sandbox, char *mem_start,
                                    size_t mem_len, size_t stack_size, uint8_t priority,
                                    int flags, thread_task_func_t function,
                                    void *arg,
                                    const char *name)
{
    sandbox->mem_start = mem_start;
    sandbox->mem_len = mem_len;

    assert(stack_size <= mem_len);

    return thread_create_detached(thread, mem_start, stack_size,
                                              priority,
                                              flags,
                                              function, arg, name);

}

int main(void)
{
    kernel_pid_t pid = thread_create_sandboxed(&sandboxed, &sandbox, mem, sizeof(mem), THREAD_STACKSIZE_MAIN,
                                              THREAD_PRIORITY_MAIN,
                                              THREAD_CREATE_STACKTEST,
                                              _sandboxed, NULL, "sandboxed");

    printf("pid=%" PRIkernel_pid "\n", pid);
    return 0;
}
