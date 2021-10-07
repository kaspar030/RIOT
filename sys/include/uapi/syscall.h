/*
 * Copyright (C) 2021 Freie Universität Berlin
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_syscall userspace syscall interface
 * @ingroup     sys
 * @brief       Provides the userspace part of the syscall interface
 *
 * @{
 *
 * @file
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef UAPI_SYSCALL_H
#define UAPI_SYSCALL_H

#include "syscall_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SYSCALL_THREAD_YIELD = 1,
    SYSCALL_THREAD_EXIT = 2,
    SYSCALL_MUTEX_LOCK,
    SYSCALL_MUTEX_UNLOCK,
    SYSCALL_PUTS,
} syscall_num_t;

void sys_mutex_lock(void *mutex);

void sys_mutex_unlock(void *mutex);

static inline void sys_puts(char *str)
{
    syscall_dispatch(SYSCALL_PUTS, (uint32_t)(intptr_t)str);
}

#ifdef __cplusplus
}
#endif
#endif /* UAPI_SYSCALL_H */
/** @} */

