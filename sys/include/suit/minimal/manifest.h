#ifndef MANIFEST_H
#define MANIFEST_H

#include <stdint.h>
#include <string.h>
#include "sys/types.h"

#include "uuid.h"

#define SUIT_MINIMAL_MANIFEST_VERSION   1
#define SUIT_MINIMAL_SLOT_NUMOF         2

/* dependend on used algorithms... */
#define SUIT_MINIMAL_DIGEST_SIZE        16
#define SUIT_MINIMAL_SIGNATURE_SIZE     16

#define SUIT_MINIMAL_URL_MAXLEN         128

#define SUIT_MINIMAL_POLICY_CHECK_DEVICE_ID     0x1

/**
 * @brief SUIT error codes
 */
typedef enum {
    SUIT_OK                     = 0,    /**< Manifest parsed and validated */
    SUIT_ERR_INVALID_MANIFEST   = -1,   /**< Manifest has wrong version or invalid size*/
    SUIT_ERR_COND               = -2,   /**< Conditionals evaluate to false */
    SUIT_ERR_SEQUENCE_NUMBER    = -3,   /**< Sequence number less or equal to
                                             current sequence number */
} suit_minimal_error_t;

typedef struct __attribute__ ((packed)) {
    uint32_t size;
    uint8_t digest[SUIT_MINIMAL_DIGEST_SIZE];
} suit_minimal_slot_t;

typedef struct __attribute__ ((packed)) {
    uint8_t signature[SUIT_MINIMAL_SIGNATURE_SIZE];
    uint8_t manifest_version;
    uint8_t policy;
    uint8_t encoded_urls_len;
    uint8_t reserved[1];
    uint32_t seq_nr;
    uuid_t vendor_id;
    uuid_t class_id;
    uuid_t device_id;
    suit_minimal_slot_t slots[SUIT_MINIMAL_SLOT_NUMOF];
    uint8_t encoded_urls[];
} suit_minimal_manifest_t;

static inline size_t suit_minimal_manifest_signlen(
    const suit_minimal_manifest_t *manifest)
{
    return sizeof(suit_minimal_manifest_t) + manifest->encoded_urls_len -
           SUIT_MINIMAL_SIGNATURE_SIZE;
}

static inline void *suit_minimal_manifest_signstart(
    suit_minimal_manifest_t *manifest)
{
    return &manifest->seq_nr;
}

ssize_t suit_minimal_encode_urls(uint8_t *outbuf, size_t outbuf_len,
                                uint8_t *urls[], size_t urls_num);

ssize_t suit_minimal_decode_url(uint8_t *out, size_t out_len, const uint8_t *encoded,
                               size_t n);

static inline ssize_t suit_minimal_url_get(
    const suit_minimal_manifest_t *manifest, uint8_t *urlbuf, size_t buflen,
    unsigned target_slot)
{
    return suit_minimal_decode_url(urlbuf, buflen, manifest->encoded_urls,
                                   target_slot);
}

int suit_minimal_manifest_validate(const suit_minimal_manifest_t *m);
void suit_minimal_manifest_print(const suit_minimal_manifest_t *m);

#endif /* MANIFEST_H */
