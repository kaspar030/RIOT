/*
 * Copyright (C) 2021 Freie Universität Berlin
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys_calls
 * @{
 * @file
 * @brief   Syscall interface implementation
 *
 * @author  Koen Zandberg <koen@bergzand.net>
 * @}
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "mutex.h"
#include "uapi/syscall.h"
#include "sched.h"
#include "thread.h"

static inline void _syscall_mutex_lock(uint32_t *args)
{
    mutex_t *mutex = (mutex_t*)(uintptr_t)args[0];
    thread_t *me = thread_get_active();
    /* Nope */
    /* Try to lock and set the thread to waiting if not possible */
    if (mutex_trylock(mutex)) {
        return; /* Mutex now locked */
    }
    sched_set_status(me, STATUS_MUTEX_BLOCKED);
    if (mutex->queue.next == MUTEX_LOCKED) {
        mutex->queue.next = (list_node_t *)&me->rq_entry;
        mutex->queue.next->next = NULL;
    }
    else {
        thread_add_to_list(&mutex->queue, me);
    }
    thread_yield_higher();
}

static inline void _syscall_mutex_unlock(uint32_t *args)
{
    mutex_t *mutex = (mutex_t*)(uintptr_t)args[0];
    mutex_unlock(mutex);
}

static inline void _syscall_puts(uint32_t *args)
{
    char *str = (char *)(uintptr_t)args[0];
    puts(str);
}

#include "fmt.h"
static inline void _syscall_putu32x(uint32_t *args)
{
#ifdef MODULE_FMT
    print_u32_hex(args[0]);
    print("\n", 1);
#else
    printf("0x%08x\n", (unsigned)args[0]);
#endif
}


void syscall_handle(unsigned syscall_num, uint32_t *args)
{
    switch(syscall_num) {
        case SYSCALL_MUTEX_LOCK:
            return _syscall_mutex_lock(args);
        case SYSCALL_MUTEX_UNLOCK:
            return _syscall_mutex_unlock(args);
        case SYSCALL_PUTS:
            return _syscall_puts(args);
        case SYSCALL_PUTU32X:
            return _syscall_putu32x(args);
    }
}
