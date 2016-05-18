/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file
 * @brief       Implementation of Cortex-M* SVC (supervisor call) interface
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "panic.h"

void svc_dispatch(unsigned int *svc_args)
{
    unsigned int svc_number;
    /*
    * Stack contains:
    * R0, R1, R2, R3, R12, R14, the return address and xPSR
    * First argument (R0) is svc_args[0]
    */
    svc_number = ((char *)svc_args[6])[-2];
    switch(svc_number)
    {
        case 0:
            puts("0");
            break;
        default:
            core_panic(PANIC_UNKNOWN_SVC, "unknown SVC nr");
            break;
    }
}
