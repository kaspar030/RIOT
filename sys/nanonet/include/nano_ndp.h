/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_nanonet
 *
 * @{
 *
 * @file
 * @brief       Messaging API for inter process communication
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */


void nano_ndp_init(void);

size_t nano_ndp_lookup(uint8_t *l3_addr, uint8_t **l2_addr);

int nano_ndp_update(nano_ctx_t *ctx);

int nano_ndp_add(uint8_t *l3_addr, uint8_t *l2_addr, size_t l2_addr_len);

void nano_ndp_dump(void);

int nano_ndp_sync(void);
