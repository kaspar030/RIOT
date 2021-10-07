/*
 * Copyright (C) 2021 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file
 * @brief       Implementation of the syscall interface
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */


#ifndef SYSCALL_ARCH_H
#define SYSCALL_ARCH_H

#include <stdint.h>
#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN

#define SYSCALL_API_INLINED

static inline __attribute__((always_inline)) uint32_t syscall_dispatch(unsigned num,
                                                                       uint32_t arg)
{
    register uint32_t arg0 __asm ("r0") = arg;
    register uint32_t result __asm ("r0");
    __asm volatile (
            "svc %[num] \n"
            : "=r" (result)
            : [num] "i" (num), "0" (arg0)
            : "memory"
            );
    return result;
}

static inline __attribute__((always_inline)) void
        syscall_set_return(uint32_t value)
{
    uint32_t *svc_args = (uint32_t*)__get_PSP();
    svc_args[0] = value;
}

static inline __attribute__((always_inline)) void
        syscall_set_return_ptr(void *ptr)
{
    uint32_t *svc_args = (uint32_t*)__get_PSP();
    svc_args[0] = (uintptr_t)ptr;
}

#endif

#ifdef __cplusplus
}
#endif

#endif /* SYSCALL_ARCH_H */

