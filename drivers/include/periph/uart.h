/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_uart UART
 * @ingroup     drivers_periph
 * @brief       Low-level UART peripheral driver
 * @{
 *
 * This is a basic UART (Universal Asynchronous Receiver Transmitter) interface
 * to allow platform independent access to the MCU's serial communication abilities.
 * This interface is intentionally designed to be as simple as possible, to allow
 * for easy implementation and maximum portability. In RIOT we only use the
 * common 8-N-1 format of the serial port (8 data bist, no parity bit, one stop bit).
 *
 * The simple interface provides capabilities to initialize the serial communication
 * module, which automatically enables for receiving data, as well as writing data
 * to the UART port, which means transmitting data. The UART device and the
 * corresponding pins need to be mapped in `RIOT/boards/ * /include/periph_conf.h`.
 * Furthermore you need to select the baudrate for initialization which is typically
 * {9600, 19200, 38400, 57600, 115200} baud. Additionally you should register a
 * callback function that is executed in interrupt context when data is being received.
 * The driver will then read the received data byte, call the registered callback
 * function and pass the received data to it via its argument. The interface enforces
 * the receiving to be impemented in an interrupt driven mode. Thus, you never now how
 * many bytes are going to be received and might want to handle that in your specific
 * callback function. The transmit function can be implemented in any way
 *
 * By default the @p UART_DEV(0) device of each board is initialized and mapped to STDIO
 * in RIOT which is used for standard input/output functions like `printf()` or
 * `puts()`.
 *
 * @file
 * @brief       Low-level UART peripheral driver interface definition
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_UART_H
#define PERIPH_UART_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Signature for receive interrupt callback
 *
 * @param[in] arg           context to the callback (optional)
 * @param[in] data          the byte that was received
 */
typedef void(*uart_rx_cb_t)(void *arg, char data);

/**
 * @brief   Interrupt context for a UART device
 * @{
 */
typedef struct uart_isr_ctx {
    uart_rx_cb_t rx_cb;     /**< data received interrupt callback */
    void *arg;              /**< argument to both callback routines */
} uart_isr_ctx_t;

typedef struct uart_driver uart_driver_t;

typedef struct uart {
    const uart_driver_t *driver;
} uart_t;

struct uart_driver {
    int(*init)(uart_t*, uint32_t, uart_rx_cb_t, void*);
    void(*write)(uart_t*, const uint8_t *, size_t);
    void(*power_on)(uart_t*);
    void(*power_off)(uart_t*);
};

/** @} */

/**
 * @brief   Initialize a given UART device
 *
 * The UART device will be initialized with the following configuration:
 * - 8 data bits
 * - no parity
 * - 1 stop bit
 * - baudrate as given
 *
 * @param[in] uart          UART device to initialize
 * @param[in] baudrate      desired baudrate in baud/s
 * @param[in] rx_cb         receive callback, executed in interrupt context once
 *                          for every byte that is received (RX buffer filled)
 * @param[in] arg           optional context passed to the callback functions
 *
 * @return                  0 on success
 * @return                  -1 on invalid UART device
 * @return                  -2 on inapplicable baudrate
 * @return                  -3 on other errors
 */
static inline int uart_init(uart_t *uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg);

/**
 * @brief   Write data from the given buffer to the specified UART device
 *
 * This function is blocking, as it will only return after @p len bytes from the
 * given buffer have been send. The way this data is send is up to the
 * implementation: active waiting, interrupt driven, DMA, etc.
 *
 * @param[in] uart          UART device to use for transmission
 * @param[in] data          data buffer to send
 * @param[in] len           number of bytes to send
 *
 */
static inline void uart_write(uart_t *uart, const uint8_t *data, size_t len);

/**
 * @brief   Power on the given UART device
 *
 * @param[in] uart          the UART device to power on
 */
static inline void uart_poweron(uart_t *uart);

/**
 * @brief Power off the given UART device
 *
 * @param[in] uart          the UART device to power off
 */
static inline void uart_poweroff(uart_t *uart);

static inline int uart_init(uart_t *uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg) {
    return uart->driver->init(uart, baudrate, rx_cb, arg);
}

static inline void uart_write(uart_t *uart, const uint8_t *data, size_t len)
{
    uart->driver->write(uart, data, len);
}

static inline void uart_poweron(uart_t *uart)
{
    uart->driver->power_on(uart);
}

static inline void uart_poweroff(uart_t *uart)
{
    uart->driver->power_off(uart);
}

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_UART_H */
/** @} */
