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

static inline void _syscall_mutex_lock(uint32_t *args)
{
    mutex_t *mutex = (mutex_t*)(uintptr_t)args[0];
    /* Nope */
    /* Try to lock and set the thread to waiting if not possible */
    mutex_lock(mutex);
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

void syscall_handle(unsigned syscall_num, uint32_t *args)
{
    switch(syscall_num) {
        case SYSCALL_MUTEX_LOCK:
            return _syscall_mutex_lock(args);
        case SYSCALL_MUTEX_UNLOCK:
            return _syscall_mutex_unlock(args);
        case SYSCALL_PUTS:
            return _syscall_puts(args);
    }
}
