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
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */
#include <stdio.h>

#include "mpu.h"
#include "cpu.h"
#include "thread.h"
#include "sandbox.h"
#include "mutex.h"
#include "uapi/syscall.h"
#include "xtimer.h"
#include "macros/units.h"


#ifndef TEST_DURATION
#define TEST_DURATION       (1000000U)
#endif

static thread_t sandboxed;
static sandbox_t sandbox;
volatile unsigned _flag = 0;
static mutex_t _mutex = MUTEX_INIT;

static char __attribute__((aligned(32))) mem[4096];

static void _timer_callback(void*arg)
{
    (void)arg;

    _flag = 1;
}

static void *_second_thread(void *arg)
{
    mutex_t *mtx = arg;

    while(1) {
        sys_mutex_lock(mtx);
    }

    return NULL;
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

    kernel_pid_t pid = thread_create_detached(thread, mem_start, stack_size,
                                              priority,
                                              flags,
                                              function, arg, name);
    thread->sandbox = sandbox;
    return pid;
}

int main(void)
{
    extern char *_sram;
    mpu_configure(
        0,                                               /* Region 0 (lowest priority) */
        (uintptr_t)&_sram,                               /* RAM base address */
        MPU_ATTR(1, AP_RW_RO, 0, 1, 0, 1, MPU_SIZE_512M) /* Allow read/write but no exec */
    );

    mpu_configure(
        1,                                               /* 1 */
        0,                               /* ROM */
        MPU_ATTR(0, AP_RO_RO, 0, 1, 0, 1, MPU_SIZE_512M) /* Allow read & exec*/
    );

    puts("starting mutex pingpong");
    kernel_pid_t pid = thread_create_sandboxed(&sandboxed, &sandbox, mem, sizeof(mem), THREAD_STACKSIZE_MAIN,
                                              THREAD_PRIORITY_MAIN - 1,
                                              THREAD_CREATE_STACKTEST |
                                              THREAD_CREATE_WOUT_YIELD |
                                              THREAD_CREATE_RUN_UNPRIVILEGED,
                                              _second_thread, &_mutex, "sandboxed mutex");

    printf("pid=%" PRIkernel_pid "\n", pid);

    /* lock the mutex, then yield to second_thread */
    mutex_lock(&_mutex);
    thread_yield_higher();

    xtimer_t timer;
    timer.callback = _timer_callback;

    uint32_t n = 0;

    xtimer_set(&timer, TEST_DURATION);
    while(!_flag) {
        mutex_unlock(&_mutex);
        n++;
    }

    printf("{ \"result\" : %"PRIu32, n);
#ifdef CLOCK_CORECLOCK
    printf(", \"ticks\" : %"PRIu32,
           (uint32_t)((TEST_DURATION/US_PER_MS) * (CLOCK_CORECLOCK/KHZ(1)))/n);
#endif
    puts(" }");

    return 0;
}
