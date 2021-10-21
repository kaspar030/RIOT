/*
 * Copyright (C) 2017 Inria
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_cortexm_common
 * @{
 *
 * @file
 * @brief           Memory definitions for the Cortex-M family
 *
 * @author          Francisco Acosta <francisco.acosta@inria.fr>
 *                  Gaëtan Harter <gaetan.harter@inria.fr>
 *
 * @}
 */

MEMORY
{
    ram  : ORIGIN = 0x20000000, LENGTH = 0x40000
    rom : ORIGIN = 0x0, LENGTH = 0x100000
}

INCLUDE cortexm_base.ld
