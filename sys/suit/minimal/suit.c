/*
 * Copyright (C) 2019 Koen Zandberg
 *               2019 Inria
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
 * @brief       SUIT manifest validation code
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include "suit/minimal/manifest.h"
#include "suit/conditions.h"
#include "riotboot/hdr.h"
#include "riotboot/slot.h"

#include "log.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static int _validate_if_bootable(const suit_minimal_manifest_t *m)
{
    const riotboot_hdr_t *hdr = riotboot_slot_get_hdr(riotboot_slot_current());
    uint32_t seq_nr = m->seq_nr;

    if (seq_nr <= hdr->version) {
        return SUIT_ERR_SEQUENCE_NUMBER;
    }

    hdr = riotboot_slot_get_hdr(riotboot_slot_other());
    if (riotboot_hdr_validate(hdr) == 0) {
        if (seq_nr <= hdr->version) {
            LOG_INFO("%"PRIu32" <= %"PRIu32"\n", seq_nr, hdr->version);
            LOG_INFO("seq_nr <= other image\n)");
            return SUIT_ERR_SEQUENCE_NUMBER;
        }
    }

    return SUIT_OK;
}

static int _validate_conditions(const suit_minimal_manifest_t *m)
{
    if (!uuid_equal(&m->vendor_id, suit_get_vendor_id())) {
        return SUIT_ERR_COND;
    }
    if (!uuid_equal(&m->class_id, suit_get_class_id())) {
        return SUIT_ERR_COND;
    }
    if (m->policy & SUIT_MINIMAL_POLICY_CHECK_DEVICE_ID) {
        if (!uuid_equal(&m->device_id, suit_get_device_id())) {
            return SUIT_ERR_COND;
        }
    }
    return SUIT_OK;
}

int suit_minimal_manifest_validate(const suit_minimal_manifest_t *m)
{
    int res;
    if (m->manifest_version != SUIT_MINIMAL_MANIFEST_VERSION) {
        return SUIT_ERR_INVALID_MANIFEST;
    }
    res = _validate_if_bootable(m);
    if (res) {
        return res;
    }
    res = _validate_conditions(m);
    if (res) {
        return res;
    }

    return SUIT_OK;
}
