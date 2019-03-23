/*
 * Copyright (C) 2018 Freie Universitšt Berlin
 * Copyright (C) 2018 Inria
 *               2019 Kaspar Schleiser <kaspar@schleiser.de>
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

#include "suit/v4/handlers.h"
#include "suit/v4/suit.h"
#include "cbor.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

int suit_v4_parse(suit_v4_manifest_t *manifest, const uint8_t *buf,
                       size_t len)
{
    manifest->buf = buf;
    manifest->len = len;

    CborParser parser;
    CborValue it, map;
    CborError err = cbor_parser_init(buf, len, SUIT_TINYCBOR_VALIDATION_MODE,
                                     &parser, &it);

    if (err != 0) {
        return SUIT_ERR_INVALID_MANIFEST;
    }

    if (!cbor_value_is_map(&it)) {
        puts("suit_v4_parse(): manifest not an array");
        return SUIT_ERR_INVALID_MANIFEST;
    }

    cbor_value_enter_container(&it, &map);

    while (!cbor_value_at_end(&map)) {
        CborValue key, value;
        key = map;
        cbor_value_advance(&map);
        value = map;

        if (!cbor_value_is_integer(&key)) {
            printf("expected integer key type, got %u\n", cbor_value_get_type(&key));
            return SUIT_ERR_INVALID_MANIFEST;
        }

        /* This check tests whether the integer fits into "int", thus the check
         * is platform dependent. This is for lack of specification of actually
         * allowed values, to be made explicit at some point. */
        int integer_key;
        if (cbor_value_get_int_checked(&key, &integer_key) == CborErrorDataTooLarge) {
            printf("integer key doesn't fit into int type\n");
        }

        printf("got key val=%i\n", integer_key);
        suit_manifest_handler_t handler = suit_manifest_get_handler(integer_key);

        if (handler) {
            int res = handler(manifest, integer_key, &value);
            if (res < 0) {
                puts("handler returned <0");
                return SUIT_ERR_INVALID_MANIFEST;
            }
        }

        cbor_value_advance(&map);
    }

    cbor_value_enter_container(&map, &it);
    return SUIT_OK;
}
