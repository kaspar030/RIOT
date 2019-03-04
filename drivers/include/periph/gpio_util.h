/*
 * Copyright (C) 2017 Philipp-Alexander Blum <philipp-blum@jakiku.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup   gpio_util GPIO I/O Utils
 * @ingroup    utils
 * @brief      GPIO I/O Utils. Same functionality as the Arduino shiftIn() in Advanced I/O.
 *
 * @{
 *
 * @file
 * @brief      GPIO I/O Utils
 *
 * @author     Philipp-Alexander Blum <philipp-blum@jakiku.de>
 */

#include "gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * @brief shift in a byte from a given data_pin. Create clock pulses on clock_pin. MSB first.
 * @param data_pin
 * @param clock_pin
 * @return the resulting uint8_t of the shift.
 */
uint8_t gpio_util_shiftin(gpio_t data_pin, gpio_t clock_pin);


#ifdef __cplusplus
}
#endif
