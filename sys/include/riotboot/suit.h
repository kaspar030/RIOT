/*
 * Copyright (C) 2018 Freie Universität Berlin
 * Copyright (C) 2018 Inria
 *               2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_riotboot_suit SUIT manifest driven riotboot updates
 * @ingroup     sys_riotboot
 * @brief       SUIT manifest driven riotboot updates
 *
 * @{
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef RIOTBOOT_SUIT_H
#define RIOTBOOT_SUIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

void riotboot_suit_run(void);
int riotboot_suit_putbytes(uint8_t *buf, size_t len, size_t offset, bool more);
uint64_t riotboot_suit_get_time(void);

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* RIOTBOOT_SUIT_H */
