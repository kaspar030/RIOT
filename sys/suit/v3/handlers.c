/*
 * Copyright (C) 2019 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup     sys_suit_v3
 * @{
 *
 * @file
 * @brief       SUIT v3
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <inttypes.h>

#include "suit/coap.h"
#include "suit/conditions.h"
#include "suit/v3/suit.h"
#include "suit/v3/handlers.h"
#include "suit/v3/policy.h"
#include "suit/v3/suit.h"
#include "riotboot/hdr.h"
#include "riotboot/slot.h"
#include <nanocbor/nanocbor.h>

#include "log.h"

static suit_manifest_handler_t _get_handler(int key,
                                            const suit_manifest_handler_t *handlers,
                                            size_t len)
{
    if (key < 0 || (size_t)key >= len) {
        return NULL;
    }
    return handlers[key];
}

int suit_handle_command_sequence(suit_v3_manifest_t *manifest, nanocbor_value_t *bseq,
                                 const suit_manifest_handler_t *handlers, size_t handlers_len)
{

    LOG_DEBUG("Handling command sequence\n");
    nanocbor_value_t it, container;

    int err = suit_cbor_subparse(bseq, &it);
    if (err < 0) {
        return err;
    }

    if ((nanocbor_enter_array(&it, &container) < 0) &&
            (nanocbor_enter_map(&it, &container) < 0)) {
        return SUIT_ERR_INVALID_MANIFEST;
    }

    while (!nanocbor_at_end(&container)) {
        int32_t key;
        if (nanocbor_get_int32(&container, &key) < 0) {
            return SUIT_ERR_INVALID_MANIFEST;
        }
        nanocbor_value_t value = container;
        suit_manifest_handler_t handler = _get_handler(key, handlers,
                                                       handlers_len);
        if (!handler) {
            return SUIT_ERR_UNSUPPORTED;
        }
        int res = handler(manifest, key, &value);
        if (res < 0) {
            LOG_DEBUG("Sequence handler error\n");
            return res;
        }
        nanocbor_skip(&container);
    }
    nanocbor_leave_container(&it, &container);

    return 0;
}
