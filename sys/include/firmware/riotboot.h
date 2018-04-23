/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_firmware Firmware Updates
 * @ingroup     sys
 * @{
 *
 * # riotboot minimal firmware update infrastructure
 *
 * ## Overview
 *
 * riotboot is the name of a minimal bootloader application and infrastructure.
 * It consists of
 *
 * - the application "riotboot" in "dist/riotboot" which serves as
 *   minimal bootloader
 *
 * - the modules "firmware" and "firmware_update" that are used
 *   in an image to handle verification and flash-writing
 *
 * - the module "ota_coap" which serves as transport for the updates
 *
 * - a tool in dist/tools/firmware which handles key generation
 *   and firmware image signing.
 *
 * - a couple of make targets to glue everything together
 *
 * ## Concept
 *
 * riotboot expects the flash to be split in three parts: one for the
 * bootloader itself, and two slots for images.
 * Of the latter, one slot holds the running (or latest) image, while the other is
 * used to write a newer image.
 *
 * The bootloader will, on reset, verify the checksum of the image slots, then
 * choose the one with the highest version number that has a valid checksum.  If
 * all slots have the same version number, the first one will be booted.  If none
 * of the slots has a valid checksum, no image will be booted and the bootloader
 * will enter "while(1);".
 *
 * Any image that the bootloader can load is supposed to be signed by the firmware
 * tool, which will take an image, sign it using tweetnacl prefixes the image with
 * corresponding metadata.
 *
 * The metadata contains
 *
 * - "RIOT" as magic number
 * - an application version
 * - an application ID
 * - a checksum
 * - a signature
 *
 * The bootloader only cares about checksum and application version. It expects a
 * running image to verify the metadata.
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
 * riotboot needs a public key to be compiled in the image for verification purposes.
 * Create a keypair using dist/tools/firmware.
 *
 * riotboot needs some board specific support, mainly slot size configuration, in
 * order to work for a specific board.
 *
 * If that support is in place, it is easiest to use the riotboot specific make
 * targets in order to get the support on the device.
 *
 * See README.md in examples/ota for examples.
 *
 * @file
 * @brief       RIOTBoot specific firmware module
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#ifndef FIRMWARE_RIOTBOOT_H
#define FIRMWARE_RIOTBOOT_H

#include "firmware.h"
#include "hashes/sha256.h"
#ifdef RIOT_VERSION
#include "firmware/update.h"
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

#define FIRMWARE_METADATA_RIOTBOOT   (0x0100)

/**
 *  @brief Length of bytes to be signed
 */
#define FIRMWARE_SIGN_BYTES         (sizeof(firmware_riotboot_t) - FIRMWARE_SIG_LEN)

typedef struct {
    firmware_metadata_t metadata;       /**< generic bootloader specific firmware info */
    uint32_t appid;                     /**< Application type ID */
    uint32_t size;                      /**< Size of firmware image */
    uint8_t hash[SHA256_DIGEST_LENGTH]; /**< SHA256 Hash of firmware image */
    uint8_t sig[FIRMWARE_SIG_LEN];      /**< Firmware Signature */
} firmware_riotboot_t;

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

typedef struct {
    firmware_update_t update;
    unsigned state;                         /**< state (see above enum)         */
    union {
        uint8_t metadata_buf[FIRMWARE_METADATA_SIZE];
        firmware_riotboot_t metadata;
    } m;
    sha256_context_t sha;
    uint8_t hash[SHA256_DIGEST_LENGTH];
} firmware_riotboot_update_t;

/**
 * @brief Rebuild the metadata struct from received bytes
 *
 * @param[in] state     ptr to the riotboot update struct
 * @param[in] buf       Buffer to read from
 * @param[in] len       Number of bytes to read
 *
 * @return              Negative on error
 * @return              Bytes consumed from buf
 */
ssize_t firmware_riotboot_recv_metadata(firmware_riotboot_update_t *state, uint8_t *buf, size_t len);

#endif /* RIOT_VERSION */

/**
 * @brief  Print formatted FW image metadata to STDIO
 *
 * @param[in] metadata  ptr to firmware metadata
 *
 */
void firmware_riotboot_print(firmware_riotboot_t *riotboot);

/**
 * @brief  Sign metadata
 *
 * @param[in] metadata  ptr to firmware metadata
 * @param[in] sk        NaCL secret signing key to use
 *
 */
int firmware_riotboot_sign(firmware_riotboot_t *riotboot, unsigned char *sk);

/**
 * @brief  Validate FW metadata signature
 *
 * @param[in] metadata  ptr to firmware metadata
 * @param[in] pk        NaCL public signing key to use
 *
 */
int firmware_riotboot_validate_signature(firmware_riotboot_t *riotboot, const unsigned char *pk);

#endif /* FIRMWARE_RIOTBOOT_H */
