/*
 * Copyright (C) 2019 Inria
 *               2019 Kaspar Schleiser <kaspar@schleiser.de>
 *               2048 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_stm32wb55 STM32WB55
 * @ingroup         cpu
 * @brief           CPU specific implementations for the STM32WB55
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "cpu_conf_common.h"

#include "vendor/stm32wbxx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   (63U)
#define CPU_FLASH_BASE                  FLASH_BASE
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
