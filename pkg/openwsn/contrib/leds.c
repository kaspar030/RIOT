/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 *
 * @author      Michael Frey <michael.frey@msasafety.com>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include "leds.h"
#include "openwsn_leds.h"

#ifdef MODULE_ZTIMER_USEC
#include "ztimer.h"
#endif
#include "board.h"
#include "periph/gpio.h"

#include <stdint.h>
#include <string.h>

/** holds the internal configuration for debug pins */
static ledpins_config_t configuration = {
    GPIO_UNDEF,
    GPIO_UNDEF,
    GPIO_UNDEF,
    GPIO_UNDEF,
    GPIO_LED_HIGH,
};

void ledpins_riot_init(const ledpins_config_t *user_config)
{
    if (IS_USED(MODULE_OPENWSN_LEDPINS)) {
        if (user_config != NULL) {
            memcpy(&configuration, user_config, sizeof(ledpins_config_t));
            leds_init();
        }
    }
    else {
        (void)user_config;
    }
}

void leds_init(void)
{
    if (IS_USED(MODULE_OPENWSN_LEDPINS)) {
        if (configuration.error != GPIO_UNDEF) {
            gpio_init(configuration.error, GPIO_OUT);
        }
        else if (configuration.sync != GPIO_UNDEF) {
            gpio_init(configuration.sync, GPIO_OUT);
        }
        else if (configuration.radio != GPIO_UNDEF) {
            gpio_init(configuration.radio, GPIO_OUT);
        }
        else if (configuration.debug != GPIO_UNDEF) {
            gpio_init(configuration.debug, GPIO_OUT);
        }
        leds_all_off();
    }
}

void leds_error_on(void)
{
    if (IS_USED(MODULE_OPENWSN_LEDPINS)) {
        if (configuration.error != GPIO_UNDEF) {
            gpio_write(configuration.error, configuration.led_on);
        }
    }
}

void leds_error_off(void)
{
    if (IS_USED(MODULE_OPENWSN_LEDPINS)) {
        if (configuration.error != GPIO_UNDEF) {
            gpio_write(configuration.error, ~configuration.led_on);
        }
    }
}

void leds_error_toggle(void)
{
    if (IS_USED(MODULE_OPENWSN_LEDPINS)) {
        if (configuration.error != GPIO_UNDEF) {
            gpio_toggle(configuration.error);
        }
    }
}

uint8_t leds_error_isOn(void)
{
    if (IS_USED(MODULE_OPENWSN_LEDPINS)) {
        uint8_t ret = 0;

        if (configuration.error != GPIO_UNDEF) {
            ret = gpio_read(configuration.error);
        }
        return ret;
    }
    else {
        return 0;
    }
}

void leds_error_blink(void)
{
    if (IS_USED(MODULE_OPENWSN_LEDPINS)) {
        /* toggle for ~10s if ztimer is used */
        for (uint8_t i = 0; i < 100; i++) {
            leds_error_toggle();
#ifdef MODULE_ZTIMER_USEC
            ztimer_sleep(ZTIMER_USEC, 100000);
#else
        for (uint32_t i = 0; i < (CLOCK_CORECLOCK / 50) ; i++){
            /* Make sure for loop is not optimized out */
            __asm__("");
        }
#endif
        }
    }
}

void leds_radio_on(void)
{
    if (IS_USED(MODULE_OPENWSN_LEDPINS)) {
        if (configuration.radio != GPIO_UNDEF) {
            gpio_write(configuration.radio, configuration.led_on);
        }
    }
}

void leds_radio_off(void)
{
    if (IS_USED(MODULE_OPENWSN_LEDPINS)) {
        if (configuration.radio != GPIO_UNDEF) {
            gpio_write(configuration.radio, ~configuration.led_on);
        }
    }
}

void leds_radio_toggle(void)
{
    if (IS_USED(MODULE_OPENWSN_LEDPINS)) {
        if (configuration.radio != GPIO_UNDEF) {
            gpio_toggle(configuration.radio);
        }
    }
}

uint8_t leds_radio_isOn(void)
{
    if (IS_USED(MODULE_OPENWSN_LEDPINS)) {
        uint8_t ret = 0;

        if (configuration.radio != GPIO_UNDEF) {
            ret = gpio_read(configuration.radio);
        }
        return ret;
    }
    else {
        return 0;
    }
}

void leds_sync_on(void)
{
    if (IS_USED(MODULE_OPENWSN_LEDPINS)) {
        if (configuration.sync != GPIO_UNDEF) {
            gpio_write(configuration.sync, configuration.led_on);
        }
    }
}

void leds_sync_off(void)
{
    if (IS_USED(MODULE_OPENWSN_LEDPINS)) {
        if (configuration.sync != GPIO_UNDEF) {
            gpio_write(configuration.sync, ~configuration.led_on);
        }
    }
}

void leds_sync_toggle(void)
{
    if (IS_USED(MODULE_OPENWSN_LEDPINS)) {
        if (configuration.sync != GPIO_UNDEF) {
            gpio_toggle(configuration.sync);
        }
    }
}

uint8_t leds_sync_isOn(void)
{
    if (IS_USED(MODULE_OPENWSN_LEDPINS)) {
        uint8_t ret = 0;

        if (configuration.sync != GPIO_UNDEF) {
            ret = gpio_read(configuration.sync);
        }
        return ret;
    }
    else {
        return 0;
    }
}

void leds_debug_on(void)
{
    if (IS_USED(MODULE_OPENWSN_LEDPINS)) {
        if (configuration.debug != GPIO_UNDEF) {
            gpio_write(configuration.debug, configuration.led_on);
        }
    }
}

void leds_debug_off(void)
{
    if (IS_USED(MODULE_OPENWSN_LEDPINS)) {
        if (configuration.debug != GPIO_UNDEF) {
            gpio_write(configuration.debug, ~configuration.led_on);
        }
    }
}

void leds_debug_toggle(void)
{
    if (IS_USED(MODULE_OPENWSN_LEDPINS)) {
        if (configuration.debug != GPIO_UNDEF) {
            gpio_toggle(configuration.debug);
        }
    }
}

uint8_t leds_debug_isOn(void)
{
    if (IS_USED(MODULE_OPENWSN_LEDPINS)) {
        uint8_t ret = 0;

        if (configuration.debug != GPIO_UNDEF) {
            ret = gpio_read(configuration.debug);
        }
        return ret;
    }
    else {
        return 0;
    }
}

void leds_all_on(void)
{
    if (IS_USED(MODULE_OPENWSN_LEDPINS)) {
        leds_error_on();
        leds_radio_on();
        leds_sync_on();
        leds_debug_on();
    }
}

void leds_all_off(void)
{
    if (IS_USED(MODULE_OPENWSN_LEDPINS)) {
        leds_error_off();
        leds_radio_off();
        leds_sync_off();
        leds_debug_off();
    }
}

void leds_all_toggle(void)
{
    if (IS_USED(MODULE_OPENWSN_LEDPINS)) {
        leds_error_toggle();
        leds_radio_toggle();
        leds_sync_toggle();
        leds_debug_toggle();
    }
}

void leds_circular_shift(void)
{
    /** not implemented */
}

void leds_increment(void)
{
    /** not implemented */
}
