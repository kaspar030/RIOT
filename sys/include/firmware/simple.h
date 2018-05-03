/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_firmware_simple Firmware Updates
 * @ingroup     sys_firmware
 * @{
 *
 * # RIOT minimal firmware update infrastructure
 *
 * ## Overview
 *
 * This module provides a simple minimal bootloader application and
 * firmware upgrade infrastructure.
 * It consists of
 *
 * - the application "riotboot" in "dist/riotboot" which serves as
 *   minimal bootloader
 *
 * - the modules "firmware_simple" and "firmware_flashwrite" that are used
 *   in an image to handle verification and flash-writing
 *
 * - the module "ota_coap" which serves as transport for the updates
 *
 * - a tool in dist/tools/firmware which handles key generation
 *   and firmware image signing.
 *
 * - a couple of make targets to glue everything together
 *
 * Any image that is flashed using this method is supposed to be signed by the
 * firmware tool, which will take an image, sign it using tweetnacl and prefixes
 * the image with corresponding metadata.
 *
 * All image transportation and verification is supposed to be done by a running
 * image.
 *
 * The user must update using a binary that has been linked & signed for the
 * desired target slot.
 * The slot that contains the running image cannot be overwritten.
 *
 * The module "firmware_update" provides a simple API that can be fed image data
 * in arbitrary block sizes.  The API is similar to stream hashing. There's
 * "firmware_update_init()", "firmware_update_put_bytes()" and
 * "firmware_update_finish()".  The module transparently handles image
 * verification and flash writing.
 *
 * A tool in dist/tools/firmware can create the necessary keys using tweetnacl,
 * and also sign/verify firmware images.
 *
 * A module in sys/firmware contains common code for firmware metadata handling.
 *
 * ## Threat vector
 *
 * If properly used, riotboot allows secure updates of running firmware.
 * Only update requests that have a valid signature will cause a write to flash.
 * Only updates with a higher version number will be considered, protecting from
 * replay or downgrade attacks, given a node is running the latest version.
 * Updates cannot put the node in undefined state if an upgrade is aborted at any
 * time or otherwise invalid.
 * Only correctly written and verified updates are considered by the bootloader.
 *
 * ## Usage
 *
 * This module needs a public key to be compiled in the image for verification
 * purposes. Create a keypair using dist/tools/firmware.
 *
 * If that support is in place, it is easiest to use the riotboot specific make
 * targets in order to get the support on the device.
 *
 * See README.md in examples/ota for examples.
 *
 * @file
 * @brief       Firmware Updates
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#ifndef FIRMWARE_SIMPLE_H
#define FIRMWARE_SIMPLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "firmware.h"
#include "hashes/sha256.h"
#ifdef RIOT_VERSION
#include "firmware/flashwrite.h"
#endif

#include "tweetnacl.h"

/**
 *  @brief Provisional length for signed hash
 */
#define FIRMWARE_SIG_LEN            (crypto_sign_BYTES)

/**
 *  @brief Length of secret Ed25519 key
 */
#define FIRMWARE_SECKEY_LEN         (crypto_sign_SECRETKEYBYTES)

/**
 *  @brief Length of public Ed25519 key
 */
#define FIRMWARE_PUBKEY_LEN         (crypto_sign_PUBLICKEYBYTES)

/**
 * @brief Firmware simple metadata type number
 */
#define FIRMWARE_METADATA_TYPE_SIMPLE   (0x0100)

/**
 *  @brief Length of bytes to be signed
 */
#define FIRMWARE_SIGN_BYTES         (sizeof(firmware_simple_t) - FIRMWARE_SIG_LEN)

/**
 * @brief Extended firmware metadata for firmware simple.
 */
typedef struct {
    firmware_metadata_t metadata;       /**< generic bootloader specific firmware info */
    uint32_t metadata_type;             /**< Type of metadata, 16 bits type,
                                          16 bit version */
    uint32_t appid;                     /**< Application type ID */
    uint32_t size;                      /**< Size of firmware image */
    uint8_t hash[SHA256_DIGEST_LENGTH]; /**< SHA256 Hash of firmware image */
    uint8_t sig[FIRMWARE_SIG_LEN];      /**< Firmware Signature */
} firmware_simple_t;

/**
 * @brief   Possible firmware update states
 */
enum {
    FIRMWARE_UPDATE_IDLE,           /**< no firmware update in progress         */
    FIRMWARE_UPDATE_INITIALIZED,    /**< firmware update in progress, awaiting
                                         verification                           */
    FIRMWARE_UPDATE_VERIFIED,       /**< firmware update in progress & verified */
};

#ifdef RIOT_VERSION

/**
 * @brief Firmware update state struct
 */
typedef struct {
    firmware_flashwrite_t writer;           /**< flashwrite state struct */
    unsigned state;                         /**< state (see above enum)  */
    union {
        uint8_t metadata_buf[FIRMWARE_METADATA_SIZE]; /**< buffer for
                                                           metadata          */
        firmware_simple_t metadata;         /**< Metadata struct as received
                                                 from the initiator          */
    } m;
    sha256_context_t sha;                   /**< SHA256 digest state struct  */
} firmware_simple_update_t;

/**
 * @brief Initiate a simple firmware update
 *
 * @param[in]   state   firmware simple update state to initialize
 *
 * returns      0 on success
 * returns      negative on failure
 */
int firmware_simple_init(firmware_simple_update_t *state);

/**
 * @brief Process received bytes
 *
 * Redirects the first @ref FIRMWARE_METADATA_SIZE bytes to the metadata buffer.
 * The following bytes are directed to the flash devices after the metadata is
 * verified
 *
 * @param[in] state     firmware simple update state
 * @param[in] bytes     bytes to process
 * @param[in] len       number of bytes to process
 *
 * @returns     0 on success
 * @returns     negative on failure
 */
int firmware_simple_putbytes(firmware_simple_update_t *state, const uint8_t *bytes, size_t len);

/**
 * @brief Finalize the firmware update
 *
 * Finalize the firmware update. Checks the digest of the flashed image and
 * writes the metadata if the check is successful
 *
 * @param[in]   state   firmware simple update struct
 *
 * @returns     zero on success
 * @returns     negative on failure
 */
int firmware_simple_finish(firmware_simple_update_t *state);

#endif /* RIOT_VERSION */

/**
 * @brief  Print formatted FW image metadata to STDIO
 *
 * @param[in] simple    ptr to firmware metadata
 *
 */
void firmware_simple_print(firmware_simple_t *simple);

/**
 * @brief  Sign metadata
 *
 * @param[in] simple    ptr to firmware metadata simple struct
 * @param[in] sk        NaCL secret signing key to use
 *
 */
int firmware_simple_sign(firmware_simple_t *simple, unsigned char *sk);

/**
 * @brief  Validate FW metadata signature
 *
 * @param[in] simple    ptr to firmware metadata simple struct
 * @param[in] pk        NaCL public signing key to use
 *
 */
int firmware_simple_validate_signature(firmware_simple_t *simple, const unsigned char *pk);

#ifdef __cplusplus
}
#endif

#endif /* FIRMWARE_SIMPLE_H */
