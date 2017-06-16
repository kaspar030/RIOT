#ifndef FIRMWARE_H
#define FIRMWARE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hashes/sha256.h"

/**
 *  @brief FIRMWARE_SIG_LEN:
 *         Provisional length for signed hash
 */
#define FIRMWARE_SIG_LEN            (SHA256_DIGEST_LENGTH)

/**
 * @brief Structure to store firmware metadata
 * @{
 */
typedef struct firmware_metadata {
    uint32_t canary;                    /**< metadata canary (always "RIOT")        */
    uint8_t hash[SHA256_DIGEST_LENGTH]; /**< SHA256 Hash of firmware image          */
    uint8_t shash[FIRMWARE_SIG_LEN];    /**< Signed SHA256                          */
    uint16_t version;                   /**< Integer representing firmware version  */
    uint32_t size;                      /**< Size of firmware image                 */
    uint32_t appid;                     /**< Integer representing the application ID*/
    uint32_t chksum;                    /**< checksum of metadata                   */
    uint32_t pad[43];
} firmware_metadata_t;
/** @} */

/**
 * @brief  Print formatted FW image metadata to STDIO.
 *
 * @param[in] metadata  ptr to firmware metadata
 *
 */
void firmware_metadata_print(firmware_metadata_t *metadata);

/**
 * @brief  Validate FW image metadata
 *
 * @param[in] metadata  ptr to firmware metadata
 *
 */
int firmware_validate_metadata(firmware_metadata_t *metadata);

/**
 * @brief  Validate FW image
 *
 * @param[in] metadata  ptr to firmware metadata
 *
 */
int firmware_validate_image(firmware_metadata_t *metadata);

/**
 * @brief  Jump to image
 *
 * @param[in] metadata  ptr to firmware metadata
 *
 */
void firmware_jump_to_image(firmware_metadata_t *metadata);

/**
 * @brief  Get currently running image slot
 */
int firmware_current_slot(void);

/**
 * @brief  Get metadata of firmware slot
 */
firmware_metadata_t *firmware_get_metadata(unsigned slot);

/**
 * @brief  Get jump-to address of firmware slot
 */
unsigned firmware_get_image_startaddr(unsigned slot);

/**
 * @brief  Boot into image in slot @p slot
 */
void firmware_jump_to_slot(unsigned slot);

/**
 * @brief  Dump firmware slot addresses
 */
void firmware_dump_slot_addrs(void);

#ifdef __cplusplus
}
#endif

#endif /* FIRMWARE_H */
