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
 * @brief       Nanonet execution context
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NANO_CTX_H
#define NANO_CTX_H

#include <stdint.h>

#include "net/nano/config.h"
#include "net/nano/dev.h"

struct nano_ctx {
    nano_dev_t *dev;

    /* buffer */
    uint8_t *buf;
    size_t buf_size;        /* maximum buffer size */
    size_t len;             /* actual used memory */

    /* l2 */
    uint8_t *src_mac;
    uint8_t *dst_mac;

#ifdef NANONET_IEEE802154
    uint16_t src_pan;
    uint16_t dst_pan;
    uint8_t src_mac_len;
    uint8_t dst_mac_len;
#endif

    /* l3 */
    uint8_t *l3_hdr_start;

    union {
        uint32_t ipv4;
        uint8_t *ipv6;
    } src_addr;

    union {
        uint32_t ipv4;
        uint8_t *ipv6;
    } dst_addr;

    /* l4 */
    uint8_t *l4_start;
    uint16_t src_port;
    uint16_t dst_port;
};

#endif /* NANO_CTX_H */
