/*
 * Copyright (C) 2019 Koen Zandberg
 *               2020 Inria
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

static int _version_handler(suit_v3_manifest_t *manifest, int key,
                            nanocbor_value_t *it)
{
    (void)manifest;
    (void)key;
    /* Validate manifest version */
    int32_t version = -1;
    if (nanocbor_get_int32(it, &version) >= 0) {
        if (version == SUIT_VERSION) {
            manifest->validated |= SUIT_VALIDATED_VERSION;
            LOG_INFO("suit: validated manifest version\n)");
            return 0;
        }
        else {
            return -1;
        }
    }
    return -1;
}

static int _seq_no_handler(suit_v3_manifest_t *manifest, int key, nanocbor_value_t *it)
{
    (void)key;

    int32_t seq_nr;

    if (nanocbor_get_int32(it, &seq_nr) < 0) {
        LOG_INFO("Unable to get sequence number\n");
        return SUIT_ERR_INVALID_MANIFEST;
    }
    const riotboot_hdr_t *hdr = riotboot_slot_get_hdr(riotboot_slot_current());
    if (seq_nr <= (int32_t)hdr->version) {
        LOG_INFO("%"PRId32" <= %"PRId32"\n", seq_nr, hdr->version);
        LOG_INFO("seq_nr <= running image\n)");
        return -1;
    }

    hdr = riotboot_slot_get_hdr(riotboot_slot_other());
    if (riotboot_hdr_validate(hdr) == 0) {
        if (seq_nr<= (int32_t)hdr->version) {
            LOG_INFO("%"PRIu32" <= %"PRIu32"\n", seq_nr, hdr->version);
            LOG_INFO("seq_nr <= other image\n)");
            return -1;
        }
    }
    LOG_INFO("suit: validated sequence number\n)");
    manifest->validated |= SUIT_VALIDATED_SEQ_NR;
    return 0;

}

static int _common_handler(suit_v3_manifest_t *manifest, int key, nanocbor_value_t *it)
{
    (void)key;
    LOG_DEBUG("Starting common section handler\n");
    return suit_handle_manifest_structure_bstr(manifest, it, suit_common_handlers,
                                               suit_common_handlers_len);
}

/* begin{code-style-ignore} */
const suit_manifest_handler_t suit_global_handlers[] = {
    [ 0] = NULL,
    [ 1] = _version_handler,
    [ 2] = _seq_no_handler,
    [ 3] = _common_handler,
};
/* end{code-style-ignore} */

const size_t suit_global_handlers_len = ARRAY_SIZE(suit_global_handlers);
