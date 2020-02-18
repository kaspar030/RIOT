/*
 * Copyright (C) 2018 Freie Universität Berlin
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup     sys_suit
 * @{
 *
 * @file
 * @brief       SUIT manifest parser library for CBOR based manifests
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "suit/v3/handlers.h"
#include "suit/v3/suit.h"
#include "suit/v3/policy.h"
#include "nanocbor/nanocbor.h"
#include "cose/sign.h"

#include "log.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

int suit_cbor_subparse(nanocbor_value_t *bseq, nanocbor_value_t *it)
{
    const uint8_t *bytes;
    size_t bytes_len = 0;
    int res = nanocbor_get_bstr(bseq, &bytes, &bytes_len);
    if (res < 0) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    nanocbor_decoder_init(it, bytes, bytes_len);
    return SUIT_OK;
}

int suit_v3_parse(suit_v3_manifest_t *manifest, const uint8_t *buf,
                  size_t len)
{
    nanocbor_value_t it;
    manifest->buf = buf;
    manifest->len = len;
    nanocbor_decoder_init(&it, buf, len);
    return suit_handle_command_sequence(manifest, &it, suit_container_handlers,
                                        suit_container_handlers_len);
}
