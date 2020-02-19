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

<<<<<<< HEAD
#include <inttypes.h>

#ifdef MODULE_SUIT_COAP
#include "suit/coap.h"
#endif

#include "kernel_defines.h"
=======
>>>>>>> suit: WIP: add common section support
#include "suit/conditions.h"
#include "suit/v3/suit.h"
#include "suit/v3/handlers.h"
#include "suit/v3/policy.h"
#include "suit/v3/suit.h"
#include "riotboot/hdr.h"
#include "riotboot/slot.h"
#include <nanocbor/nanocbor.h>

#include "log.h"

static int _component_handler(suit_v3_manifest_t *manifest, int key,
                              nanocbor_value_t *it)
{
    (void)manifest;
    (void)key;

    nanocbor_value_t arr;

    LOG_DEBUG("storing components\n)");
    if (nanocbor_enter_array(it, &arr) < 0) {
        LOG_DEBUG("components field not an array\n");
        return -1;
    }
    unsigned n = 0;
    while (!nanocbor_at_end(&arr)) {
        nanocbor_value_t map;
        if (n < SUIT_V3_COMPONENT_MAX) {
            manifest->components_len += 1;
        }
        else {
            LOG_DEBUG("too many components\n)");
            return SUIT_ERR_INVALID_MANIFEST;
        }

        if (nanocbor_enter_map(&arr, &map) < 0) {
            LOG_DEBUG("suit _v3_parse(): manifest not a map!\n");
            return SUIT_ERR_INVALID_MANIFEST;
        }

        suit_v3_component_t *current = &manifest->components[n];

        while (!nanocbor_at_end(&map)) {

            /* handle key, value */
            int32_t integer_key;
            if (nanocbor_get_int32(&map, &integer_key) < 0) {
                return SUIT_ERR_INVALID_MANIFEST;
            }

            switch (integer_key) {
                case SUIT_COMPONENT_IDENTIFIER:
                    current->identifier = map;
                    break;
                case SUIT_COMPONENT_SIZE:
                    LOG_DEBUG("skipping SUIT_COMPONENT_SIZE");
                    break;
                case SUIT_COMPONENT_DIGEST:
                    current->digest = map;
                    break;
                default:
                    LOG_DEBUG("ignoring unexpected component data (nr. %" PRIi32 ")\n",
                              integer_key);
            }
            nanocbor_skip(&map);

            LOG_DEBUG("component %u parsed\n", n);
        }
        nanocbor_leave_container(&arr, &map);
        n++;
    }
    nanocbor_leave_container(it, &arr);

    manifest->state |= SUIT_MANIFEST_HAVE_COMPONENTS;

    int target_slot = riotboot_slot_other();
    riotboot_flashwrite_init(manifest->writer, target_slot);

    int res = -1;

    if (0) {}
#ifdef MODULE_SUIT_COAP
    else if (strncmp(manifest->urlbuf, "coap://", 7) == 0) {
        res = suit_coap_get_blockwise_url(manifest->urlbuf, COAP_BLOCKSIZE_64,
                                          suit_flashwrite_helper,
                                          manifest);
    }
#endif
#ifdef MODULE_SUIT_V3_TEST
    else if (strncmp(manifest->urlbuf, "test://", 7) == 0) {
        res = SUIT_OK;
    }

#endif
    else {
        LOG_WARNING("suit: unsupported URL scheme!\n)");
        return res;
    }

    LOG_DEBUG("storing components done\n)");

    return 0;
}

static int _dependencies_handler(suit_v3_manifest_t *manifest, int key,
                                 nanocbor_value_t *it)
{
    (void)manifest;
    (void)key;
    (void)it;
    /* No dependency support */
    return SUIT_ERR_UNSUPPORTED;
}


static int _common_sequence_handler(suit_v3_manifest_t *manifest, int key, nanocbor_value_t *it)
{
    (void)key;
    LOG_DEBUG("Starting conditional sequence handler\n");
    return suit_handle_manifest_structure_bstr(manifest, it, suit_sequence_handlers,
                                        suit_sequence_handlers_len);
}

/* begin{code-style-ignore} */
const suit_manifest_handler_t suit_common_handlers[] = {
    [ 0] = NULL,
    [ 1] = _dependencies_handler,
    [ 2] = _component_handler,
    [ 3] = NULL,
    [ 4] = _common_sequence_handler,
};
/* end{code-style-ignore} */

const size_t suit_common_handlers_len = ARRAY_SIZE(suit_common_handlers);
