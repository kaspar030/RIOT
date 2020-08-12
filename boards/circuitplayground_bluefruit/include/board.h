/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_circuitplayground_bluefruit
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Circuit Playground Bluefruit
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "board_common.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin configuration
 * @{
 */
 /** @brief The red channel of the LED */
#define LED0_PIN            GPIO_PIN(1, 14)
 /** @brief The green channel of the LED */
#define LED1_PIN            GPIO_PIN(0, 22)
 /** @brief The blue channel of the LED */
#define LED2_PIN            GPIO_PIN(0, 24)

/** @} */

/**
 * @name    LED access macros
 * @{
 */

/** Enable LED's red channel */
#define LED0_ON gpio_clear(LED0_PIN)
/** Disable LED's red channel */
#define LED0_OFF gpio_set(LED0_PIN)
/** Toggle LED's red channel */
#define LED0_TOGGLE gpio_toggle(LED0_PIN)

/** Enable LED's green channel */
#define LED1_ON gpio_clear(LED1_PIN)
/** Disable LED's green channel */
#define LED1_OFF gpio_set(LED1_PIN)
/** Toggle LED's green channel */
#define LED1_TOGGLE gpio_toggle(LED1_PIN)

/** Enable LED's blue channel */
#define LED2_ON gpio_clear(LED2_PIN)
/** Disable LED's blue channel */
#define LED2_OFF gpio_set(LED2_PIN)
/** Toggle LED's blue channel */
#define LED2_TOGGLE gpio_toggle(LED2_PIN)

/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
/** @brief The button labelled RESET */
#define BTN0_PIN            GPIO_PIN(0, 18)
/** @brief The GPIO pin mode of the button labelled RESET */
#define BTN0_MODE           GPIO_IN_PU
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
