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

static int _validate_uuid(suit_v3_manifest_t *manifest, nanocbor_value_t *it, uuid_t *uuid)
{
    (void)manifest;
    const uint8_t *uuid_manifest_ptr;
    size_t len = sizeof(uuid_t);
    char uuid_str[UUID_STR_LEN + 1];
    char uuid_str2[UUID_STR_LEN + 1];
    if (nanocbor_get_bstr(it, &uuid_manifest_ptr, &len) < 0) {
        return SUIT_ERR_INVALID_MANIFEST;
    }

    uuid_to_string((uuid_t *)uuid_manifest_ptr, uuid_str);
    uuid_to_string(uuid, uuid_str2);
    LOG_INFO("Comparing %s to %s from manifest\n", uuid_str2, uuid_str);
    return uuid_equal(uuid, (uuid_t *)uuid_manifest_ptr) ? 0 : -1;
}

static int _cond_vendor_handler(suit_v3_manifest_t *manifest, int key, nanocbor_value_t *it)
{
    (void)key;
    LOG_INFO("validating vendor ID\n");
    int rc = _validate_uuid(manifest, it, suit_get_vendor_id());
    if (rc == SUIT_OK) {
        LOG_INFO("validating vendor ID: OK\n");
        manifest->validated |= SUIT_VALIDATED_VENDOR;
    }
    return rc;
}

static int _cond_class_handler(suit_v3_manifest_t *manifest, int key, nanocbor_value_t *it)
{
    (void)key;
    LOG_INFO("validating class id\n");
    int rc = _validate_uuid(manifest, it, suit_get_class_id());
    if (rc == SUIT_OK) {
        LOG_INFO("validating class id: OK\n");
        manifest->validated |= SUIT_VALIDATED_CLASS;
    }
    return rc;
}

static int _cond_comp_offset(suit_v3_manifest_t *manifest, int key, nanocbor_value_t *it)
{
    (void)manifest;
    (void)key;
    uint32_t offset;
    nanocbor_get_uint32(it, &offset);
    uint32_t other_offset = (uint32_t)riotboot_slot_get_hdr(riotboot_slot_other()) \
                            - CPU_FLASH_BASE;
    LOG_INFO("Comparing manifest offset %u with other slot offset %u\n",
           (unsigned)offset, (unsigned)other_offset);
    return other_offset == offset ? 0 : -1;
}

static int _dtv_set_comp_idx(suit_v3_manifest_t *manifest, int key, nanocbor_value_t *it)
{
    (void)key;
    if (nanocbor_get_type(it) == NANOCBOR_TYPE_FLOAT) {
        LOG_DEBUG("_dtv_set_comp_idx() ignoring boolean and floats\n)");
        nanocbor_skip(it);
    }
    else if (nanocbor_get_int32(it, &manifest->component_current) < 0) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    LOG_DEBUG("Setting component index to %d\n", (int)manifest->component_current);
    return 0;
}

static int _dtv_run_seq_cond(suit_v3_manifest_t *manifest, int key, nanocbor_value_t *it)
{
    (void)key;
    LOG_DEBUG("Starting conditional sequence handler\n");
    return suit_handle_manifest_structure_bstr(manifest, it, suit_sequence_handlers,
                                               suit_sequence_handlers_len);
}

static int _param_get_uri_list(suit_v3_manifest_t *manifest, nanocbor_value_t *it)
{
    LOG_DEBUG("got url list\n");
    manifest->components[manifest->component_current].url = *it;
    return 0;
}
static int _param_get_digest(suit_v3_manifest_t *manifest, nanocbor_value_t *it)
{
    LOG_DEBUG("got digest\n");
    manifest->components[manifest->component_current].digest = *it;
    return 0;
}

static int _param_get_img_size(suit_v3_manifest_t *manifest, nanocbor_value_t *it)
{
    int res = nanocbor_get_uint32(it, &manifest->components[0].size);
    if (res < 0) {
        LOG_DEBUG("error getting image size\n");
        return res;
    }
    return res;
}

static int _dtv_set_param(suit_v3_manifest_t *manifest, int key, nanocbor_value_t *it)
{
    (void)key;
    /* `it` points to the entry of the map containing the type and value */
    nanocbor_value_t map;

    nanocbor_enter_map(it, &map);

    while (!nanocbor_at_end(&map)) {
        /* map points to the key of the param */
        int32_t param_key;
        nanocbor_get_int32(&map, &param_key);
        LOG_DEBUG("Setting component index to %" PRIi32 "\n", manifest->component_current);
        LOG_DEBUG("param_key=%" PRIi32 "\n", param_key);
        int res;
        switch (param_key) {
            case 6: /* SUIT URI LIST */
                res = _param_get_uri_list(manifest, &map);
                break;
            case 11: /* SUIT DIGEST */
                res = _param_get_digest(manifest, &map);
                break;
            case 12: /* SUIT IMAGE SIZE */
                res = _param_get_img_size(manifest, &map);
                break;
            default:
                res = SUIT_ERR_UNSUPPORTED;
        }

        nanocbor_skip(&map);

        if (res) {
            return res;
        }
    }
    return SUIT_OK;
}

static int _dtv_fetch(suit_v3_manifest_t *manifest, int key, nanocbor_value_t *_it)
{
    (void)key; (void)_it; (void)manifest;
    LOG_DEBUG("_dtv_fetch() key=%i\n", key);

    const uint8_t *url;
    size_t url_len;

    /* TODO: there must be a simpler way */
    {
        /* the url list is a binary sequence containing a cbor array of
         * (priority, url) tuples (represented as array with length two)
         * */

        nanocbor_value_t it;

        /* open sequence with cbor parser */
        int err = suit_cbor_subparse(&manifest->components[0].url, &it);
        if (err < 0) {
            LOG_DEBUG("subparse failed\n)");
            return err;
        }

        nanocbor_value_t url_it;
        /* enter container, confirm it is an array, too */
        if (nanocbor_enter_array(&it, &url_it) < 0) {
            LOG_DEBUG("url list no array\n)");
            return SUIT_ERR_INVALID_MANIFEST;
        }

        nanocbor_value_t url_value_it;
        if (nanocbor_enter_array(&url_it, &url_value_it) < 0) {
            LOG_DEBUG("url entry no array\n)");
            return SUIT_ERR_INVALID_MANIFEST;
        }

        /* expect two entries: priority as int, url as byte string. bail out if not. */
        uint32_t prio;
        /* check that first array entry is an int (the priority of the url) */
        if (nanocbor_get_uint32(&url_value_it, &prio) < 0) {
            LOG_DEBUG("expected URL priority (int), got %d\n",
                      nanocbor_get_type(&url_value_it));
            return -1;
        }
        LOG_DEBUG("URL priority %"PRIu32"\n", prio);

        int res = nanocbor_get_tstr(&url_value_it, &url, &url_len);
        if (res < 0) {
            LOG_DEBUG("error parsing URL\n)");
            return SUIT_ERR_INVALID_MANIFEST;
        }
        if (url_len >= manifest->urlbuf_len) {
            LOG_INFO("url too large: %u>%u\n)", (unsigned)url_len,
                     (unsigned)manifest->urlbuf_len);
            return SUIT_ERR_UNSUPPORTED;
        }
        memcpy(manifest->urlbuf, url, url_len);
        manifest->urlbuf[url_len] = '\0';

        nanocbor_leave_container(&url_it, &url_value_it);
        nanocbor_leave_container(&it, &url_it);
    }

    LOG_DEBUG("_dtv_fetch() fetching \"%s\" (url_len=%u)\n", manifest->urlbuf,
              (unsigned)url_len);

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

    if (res) {
        LOG_INFO("image download failed\n)");
        return res;
    }

    const uint8_t *digest;
    size_t digest_len;

    res = nanocbor_get_bstr(&manifest->components[0].digest, &digest, &digest_len);
    if (res < 0) {
        return SUIT_ERR_INVALID_MANIFEST;
    }

    /* "digest" points to a 36 byte string that includes the digest type.
     * riotboot_flashwrite_verify_sha256() is only interested in the 32b digest,
     * so shift the pointer accordingly.
     */
    res = riotboot_flashwrite_verify_sha256(digest + 4, manifest->components[0].size,
                                            target_slot);
    if (res) {
        LOG_INFO("image verification failed\n");
        return res;
    }

    manifest->state |= SUIT_MANIFEST_HAVE_IMAGE;

    return SUIT_OK;
}

/* begin{code-style-ignore} */
const suit_manifest_handler_t suit_sequence_handlers[] = {
    [ 0] = NULL,
    [ 1] = _cond_vendor_handler,
    [ 2] = _cond_class_handler,
    [10] = _cond_comp_offset,
    /* Directives */
    [11] = _dtv_set_comp_idx,
    /* [12] = _dtv_set_man_idx, */
    /* [13] = _dtv_run_seq, */
    [14] = _dtv_run_seq_cond,
    [16] = _dtv_set_param,
    [20] = _dtv_fetch,
    /* [22] = _dtv_run, */
};
/* end{code-style-ignore} */

const size_t suit_sequence_handlers_len = ARRAY_SIZE(suit_sequence_handlers);
