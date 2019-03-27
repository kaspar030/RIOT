#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "suit/minimal/manifest.h"

void suit_minimal_manifest_print(const suit_minimal_manifest_t *m)
{
    printf("seq nr: 0x%08x\n", (unsigned)m->seq_nr);
    for (unsigned slot = 0; slot < 2; slot++) {
        uint8_t url[SUIT_MINIMAL_URL_MAXLEN];
        suit_minimal_url_get(m, url, SUIT_MINIMAL_URL_MAXLEN, slot);
        printf("url slot %u: \"%s\"\n", slot, url);
    }
}

ssize_t suit_minimal_encode_urls(uint8_t *outbuf, size_t outbuf_len,
                                uint8_t *urls[], size_t urls_num)
{
    size_t prefix_len;
    uint8_t *outbuf_pos;

    /* count number of prefix chars all urls have in common */
    for (prefix_len = 0;; prefix_len++) {
        uint8_t c = urls[0][prefix_len];
        if (!c) {
            break;
        }

        for (size_t i = 1; i < urls_num; i++) {
            if (urls[i][prefix_len] != c) {
                goto done;
            }
        }
    }

done:

    if ((prefix_len + 3) > outbuf_len) {
        return -ENOSPC;
    }

    outbuf_pos = outbuf;
    *outbuf_pos++ = (uint8_t)prefix_len;
    memcpy(outbuf_pos, urls[0], prefix_len);

    outbuf_pos += prefix_len;
    for (size_t i = 0; i < urls_num; i++) {
        size_t to_copy = strlen((char *)urls[0]) - prefix_len;
        *outbuf_pos++ = (uint8_t)to_copy;
        memcpy(outbuf_pos, urls[i] + prefix_len, to_copy);
        outbuf_pos += to_copy;
    }

    return outbuf_pos - outbuf;
}

ssize_t suit_minimal_decode_url(uint8_t *out, size_t out_len, const uint8_t *encoded,
                               size_t n)
{
    uint8_t len = *encoded++;
    size_t outlen = len;

    if (out_len < len) {
        return -ENOSPC;
    }

    memcpy(out, encoded, len);
    out += len;
    encoded += len;
    for (unsigned i = 0; i < n; i++) {
        len = *encoded++;
        encoded += len;
    }
    len = *encoded++;
    outlen += len;

    if ((out_len + 1) < len) {
        return -ENOSPC;
    }

    strncpy((char *)out, (char *)encoded, len);
    return outlen;
}
