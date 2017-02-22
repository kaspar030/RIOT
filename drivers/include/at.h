/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef AT_H
#define AT_H

#include <stdint.h>

#include "isrpipe.h"
#include "periph/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    isrpipe_t isrpipe;
    uart_t uart;
} at_dev_t;

int at_dev_init(at_dev_t *dev, uart_t uart, uint32_t baudrate, char *buf, size_t bufsize);
int at_send_cmd_wait_ok(at_dev_t *dev, const char *command, uint32_t timeout);
ssize_t at_send_cmd_get_resp(at_dev_t *dev, const char *command, char *resp_buf, size_t len, uint32_t timeout);
ssize_t at_send_cmd_get_lines(at_dev_t *dev, const char *command, char *resp_buf, size_t len, uint32_t timeout);

int at_expect_bytes(at_dev_t *dev, const char *bytes, size_t len, uint32_t timeout);
int at_send_cmd(at_dev_t *dev, const char *command, uint32_t timeout);
ssize_t at_readline(at_dev_t *dev, char *resp_buf, size_t len, uint32_t timeout);
void at_drain(at_dev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* AT_H */
