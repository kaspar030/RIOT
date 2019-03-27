#include <stdio.h>
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
