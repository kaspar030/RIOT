/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef SIM8XX_H
#define SIM8XX_H

#include "at.h"
#include "mutex.h"
#include "periph/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SIM8XX_UART_BUFSIZE (128U)
#define SIM8XX_SERIAL_TIMEOUT (1000000U)

typedef struct {
    at_dev_t at_dev;
    mutex_t mutex;
    char buf[SIM8XX_UART_BUFSIZE];
} sim8xx_t;

typedef struct {
    uart_t uart;
    uint32_t baudrate;
} sim8xx_params_t;

/**
 * @brief   Initialize SIM8XX device
 *
 * @param[in]   simdev  device struct to operate on
 * @param[in]   params  parameter struct used to initialize @p simdev
 *
 * @returns     0 on success
 * @returns     <0 otherwise
 */
int sim8xx_init(sim8xx_t *simdev, const sim8xx_params_t *params);

/**
 * @brief   Set SIM card PIN
 *
 * @param[in]   simdev  device struct to operate on
 * @param[in]   pin     PIN to set
 *
 * @returns     0 on success
 * @returns     -1 on error
 */
int sim8xx_set_pin(sim8xx_t *simdev, const char *pin);

/**
 * @brief   Test if SIM card needs PIN
 *
 * @param[in]   simdev  device struct to operate on
 *
 * @returns     0 if SIM doesn't need pin
 * @returns     1 if SIM needs to be unlocked with PIN
 */
int sim8xx_check_pin(sim8xx_t *simdev);

/**
 * @brief   Initialize GPRS connection
 *
 * @param[in]   simdev  device struct to operate on
 * @param[in]   apn     APN used to connect
 *
 * @returns     0 on success
 * @returns     <0 otherwise
 */
int sim8xx_gprs_init(sim8xx_t *simdev, const char *apn);

/**
 * @brief   Check if modem is registered to network
 *
 * @param[in]   simdev  device to operate on
 *
 * @returns     0 if registered
 * @returns     1 otherwise
 */
int sim8xx_reg_check(sim8xx_t *simdev);

size_t sim8xx_reg_get(sim8xx_t *simdev, char *buf, size_t len);
int sim8xx_signal_get(sim8xx_t *simdev, unsigned *csq, unsigned *ber);
int sim8xx_imei_get(sim8xx_t *simdev, char *buf, size_t len);
uint32_t sim8xx_gprs_getip(sim8xx_t *simdev);
void sim8xx_print_status(sim8xx_t *simdev);

ssize_t sim8xx_http_get(sim8xx_t *simdev, const char *url, uint8_t *resultbuf, size_t len);
ssize_t sim8xx_http_post(sim8xx_t *simdev,
                         const char *url,
                         const uint8_t *data, size_t data_len,
                         uint8_t *resultbuf, size_t result_len);

#ifdef __cplusplus
}
#endif

#endif /* SIM8XX_H */
