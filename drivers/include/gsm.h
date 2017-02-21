/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef GSM_H
#define GSM_H

#include "at.h"
#include "mutex.h"
#include "periph/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GSM_UART_BUFSIZE (128U)
#define GSM_SERIAL_TIMEOUT (1000000U)

typedef struct {
    at_dev_t at_dev;
    mutex_t mutex;
    char buf[GSM_UART_BUFSIZE];
} gsm_t;

typedef struct {
    uart_t uart;
    uint32_t baudrate;
} gsm_params_t;

/**
 * @brief   Initialize GSM device
 *
 * @param[in]   gsmdev  device struct to operate on
 * @param[in]   params  parameter struct used to initialize @p gsmdev
 *
 * @returns     0 on success
 * @returns     <0 otherwise
 */
int gsm_init(gsm_t *gsmdev, const gsm_params_t *params);

/**
 * @brief   Set SIM card PIN
 *
 * @param[in]   gsmdev  device struct to operate on
 * @param[in]   pin     PIN to set
 *
 * @returns     0 on success
 * @returns     -1 on error
 */
int gsm_set_pin(gsm_t *gsmdev, const char *pin);

/**
 * @brief   Test if SIM card needs PIN
 *
 * @param[in]   gsmdev  device struct to operate on
 *
 * @returns     0 if SIM doesn't need pin
 * @returns     1 if SIM needs to be unlocked with PIN
 */
int gsm_check_pin(gsm_t *gsmdev);

/**
 * @brief   Initialize GPRS connection
 *
 * @param[in]   gsmdev  device struct to operate on
 * @param[in]   apn     APN used to connect
 *
 * @returns     0 on success
 * @returns     <0 otherwise
 */
int gsm_gprs_init(gsm_t *gsmdev, const char *apn);

/**
 * @brief   Check if modem is registered to network
 *
 * @param[in]   gsmdev  device to operate on
 *
 * @returns     0 if registered
 * @returns     1 otherwise
 */
int gsm_reg_check(gsm_t *gsmdev);

size_t gsm_reg_get(gsm_t *gsmdev, char *buf, size_t len);
int gsm_signal_get(gsm_t *gsmdev, unsigned *csq, unsigned *ber);
int gsm_imei_get(gsm_t *gsmdev, char *buf, size_t len);
uint32_t gsm_gprs_getip(gsm_t *gsmdev);
void gsm_print_status(gsm_t *gsmdev);

ssize_t gsm_http_get(gsm_t *gsmdev, const char *url, uint8_t *resultbuf, size_t len);
ssize_t gsm_http_post(gsm_t *gsmdev,
                         const char *url,
                         const uint8_t *data, size_t data_len,
                         uint8_t *resultbuf, size_t result_len);

int gsm_gps_get_loc(gsm_t *gsmdev, uint8_t *buf, size_t len);
size_t gsm_cmd(gsm_t *gsmdev, const char *cmd, uint8_t *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* GSM_H */
