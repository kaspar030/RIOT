/*
 * Copyright (C) 2021 Freie Universität Berlin
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_syscall syscall interface
 * @ingroup     sys
 * @brief       Provides a generic syscall interface
 *
 * @{
 *
 * @file
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>
#include "syscall_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

void syscall_handle(unsigned syscall_num, uint32_t *args);

#ifdef __cplusplus
}
#endif
#endif /* SYSCALL_H */
/** @} */
