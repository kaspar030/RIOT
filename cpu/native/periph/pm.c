/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     native_cpu
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief       native Power Management implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "periph/pm.h"
#include "native_internal.h"
#include "async_read.h"
#include "tty_uart.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifndef MODULE_PM_CUSTOM
void pm_set_lowest(void)
{
    _native_in_syscall++; // no switching here
    real_pause();
    _native_in_syscall--;

    if (_native_sigpend > 0) {
        _native_in_syscall++;
        _native_syscall_leave();
    }
}
#endif

void pm_off(void)
{
    puts("\nnative: exiting");
    real_exit(EXIT_SUCCESS);
}

void pm_reboot(void)
{
    printf("\n\n\t\t!! REBOOT !!\n\n");

    native_async_read_cleanup();

    if (real_execve(_native_argv[0], _native_argv, NULL) == -1) {
        err(EXIT_FAILURE, "reboot: execve");
    }

    errx(EXIT_FAILURE, "reboot: this should not have been reached");
}
