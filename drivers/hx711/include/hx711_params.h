/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_hx711
 * @{
 *
 * @file
 * @brief       Default parameters for HX711 scale sensors
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef HX711_PARAMS_H
#define HX711_PARAMS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default configuration parameters for the HX711 driver
 * @{
 */
#ifndef HX711_PARAM_DOUT
#define HX711_PARAM_DOUT        (GPIO_PIN(0, 0))    /**< Default DOUT pin */
#endif
#ifndef HX711_PARAM_PD_SCK
#define HX711_PARAM_PD_SCK      (GPIO_PIN(0, 1))    /**< Default PD_SCK pin */
#endif
#ifndef HX711_PARAM_GAIN
#define HX711_PARAM_GAIN        (HX711_GAIN_128)    /**< Default gain value */
#endif

#ifndef HX711_PARAMS
#define HX711_PARAMS            { .dout = HX711_PARAM_DOUT,     \
                                  .sck = HX711_PARAM_PD_SCK,    \
                                  .gain = HX711_PARAM_GAIN,         \
                                }
#endif
/** @} */

/**
 * @brief   HX711 configuration
 */
static const hx711_params_t hx711_params[] = {
    HX711_PARAMS
};
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* HX711_PARAMS_H */
/** @} */
