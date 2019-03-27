#ifndef MANIFEST_H
#define MANIFEST_H

#include <stdint.h>
#include <string.h>

#define SUIT_MINIMAL_SLOT_NUMOF 2
/* dependend on used algorithms... */
#define SUIT_MINIMAL_DIGEST_SIZE        16
#define SUIT_MINIMAL_SIGNATURE_SIZE     16

#define SUIT_MINIMAL_URL_MAXLEN         128

typedef struct __attribute__ ((packed)) {
    uint32_t size;
    uint8_t url[SUIT_MINIMAL_URL_MAXLEN];
    uint8_t digest[SUIT_MINIMAL_DIGEST_SIZE];
} suit_minimal_slot_t;

typedef struct __attribute__ ((packed)) {
    uint8_t signature[SUIT_MINIMAL_SIGNATURE_SIZE];
    uint32_t seq_nr;
    uint32_t vendor_id;
    uint32_t class_id;
    uint32_t device_id;
    uint8_t manifest_version;
    suit_minimal_slot_t slots[SUIT_MINIMAL_SLOT_NUMOF];
    uint8_t url_len[SUIT_MINIMAL_SLOT_NUMOF];
} suit_minimal_manifest_t;

static inline size_t suit_minimal_manifest_signlen(const suit_minimal_manifest_t *manifest)
{
    (void)manifest;
    return sizeof(suit_minimal_manifest_t) - SUIT_MINIMAL_SIGNATURE_SIZE;
}

static inline void *suit_minimal_manifest_signstart(suit_minimal_manifest_t *manifest)
{
    return &manifest->seq_nr;
}

static inline ssize_t suit_minimal_url_get(const suit_minimal_manifest_t *manifest,
                                       uint8_t *urlbuf, size_t buflen, unsigned target_slot)
{
    if (manifest->url_len[target_slot] > (buflen + 1))  {
        return -1;
    }

    /* copy url */
    memcpy(urlbuf, manifest->slots[target_slot].url, manifest->url_len[target_slot]);

    /* null terminate */
    urlbuf[manifest->url_len[target_slot]] = '\0';

    return manifest->url_len[target_slot] + 1;
}

void suit_minimal_manifest_print(const suit_minimal_manifest_t *m);

#endif /* MANIFEST_H */
