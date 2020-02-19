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

#include "cose/sign.h"

#include "public_key.h"
#include "log.h"

static int _auth_handler(suit_v3_manifest_t *manifest, int key,
                         nanocbor_value_t *it)
{
    (void)key;
    const uint8_t *cose_buf;
    size_t cose_len = 0;
    /* It is a list of cose signatures */
    int res = nanocbor_get_bstr(it, &cose_buf, &cose_len);

    if (res < 0) {
        LOG_INFO("Unable to get COSE signature\n");
        return SUIT_ERR_INVALID_MANIFEST;
    }
    res = cose_sign_decode(&manifest->verify, cose_buf, cose_len);
    res = 0;
    if (res < 0) {
        LOG_INFO("Unable to parse COSE signature\n");
        return SUIT_ERR_INVALID_MANIFEST;
    }
    return 0;
}

static int _manifest_handler(suit_v3_manifest_t *manifest, int key,
                             nanocbor_value_t *it)
{
    (void)key;
    const uint8_t *manifest_buf;
    size_t manifest_len;

    nanocbor_value_t cbor_buf = *it;

    nanocbor_get_bstr(&cbor_buf, &manifest_buf, &manifest_len);

    /* Validate the COSE struct first now that we have the payload */
    cose_sign_decode_set_payload(&manifest->verify, manifest_buf, manifest_len);

    /* Iterate over signatures, should only be a single signature */
    cose_signature_dec_t signature;

    cose_sign_signature_iter_init(&signature);
    if (!cose_sign_signature_iter(&manifest->verify, &signature)) {
        //return SUIT_ERR_INVALID_MANIFEST;
    }

    /* Initialize key from hardcoded public key */
    cose_key_t pkey;
    cose_key_init(&pkey);
    cose_key_set_keys(&pkey, COSE_EC_CURVE_ED25519, COSE_ALGO_EDDSA,
                      (uint8_t *)public_key, NULL, NULL);

    LOG_INFO("suit: verifying manifest signature... (skipped)\n");
    int verification = 0; /*cose_sign_verify(&manifest->verify, &signature,
                                        &pkey, manifest->validation_buf,
                                        SUIT_COSE_BUF_SIZE);*/
    if (verification != 0) {
        LOG_INFO("Unable to validate signature\n");
        return SUIT_ERR_SIGNATURE;
    }

    LOG_DEBUG("Starting global sequence handler\n");
    return suit_handle_manifest_structure_bstr(manifest, it, suit_global_handlers,
                                                suit_global_handlers_len);
}

/* begin{code-style-ignore} */
const suit_manifest_handler_t suit_container_handlers[] = {
    [ 0] = NULL,
    [ 2] = _auth_handler,
    [ 3] = _manifest_handler,
};
/* end{code-style-ignore} */

const size_t suit_container_handlers_len = ARRAY_SIZE(suit_container_handlers);
