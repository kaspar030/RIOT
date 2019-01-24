/*
 * Copyright (C) 2017 Philipp-Alexander Blum <philipp-blum@jakiku.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_hx711 HX711 digital scale sensor
 * @ingroup     drivers_sensors
 * @brief       Driver for the HX711 digital scale sensor
 *
 * @{
 * @file
 * @brief       HX711 driver
 *
 * @author      Philipp-Alexander Blum <philipp-blum@jakiku.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef HX711_H
#define HX711_H

#include <stdint.h>
#include "periph/gpio.h"

typedef enum {
    HX711_GAIN_128 = 1,
    HX711_GAIN_32 = 2,
    HX711_GAIN_64 = 3
} hx711_gain_t;

typedef struct {
    gpio_t dout;
    gpio_t sck;
    hx711_gain_t gain;
} hx711_params_t;

typedef struct {
    hx711_params_t params;
    int32_t offset;
} hx711_t;

void hx711_init(hx711_t *dev, const hx711_params_t *params);
void hx711_set_gain(hx711_t *dev, hx711_gain_t gain);
int32_t hx711_read(hx711_t *dev);
int32_t hx711_read_average(hx711_t *dev, uint8_t times);
int32_t hx711_get_value(hx711_t *dev, uint8_t times);
void hx711_tare(hx711_t *dev, uint8_t times);
void hx711_set_offset(hx711_t *dev, int32_t offset);
int32_t hx711_get_offset(hx711_t *dev);
void hx711_power_down(hx711_t *dev);
void hx711_power_up(hx711_t *dev);

#endif /* HX711_H */
