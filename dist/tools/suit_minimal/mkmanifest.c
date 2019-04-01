#include <stdio.h>
#include <stdlib.h>

#include "suit/minimal/manifest.h"
#include "uuid.h"

#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int to_file(const char *filename, void *buf, size_t len)
{
    int fd;

    if (strcmp("-", filename)) {
        fd = open(filename, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
    }
    else {
        fd = STDOUT_FILENO;
    }

    if (fd > 0) {
        ssize_t res = write(fd, buf, len);
        close(fd);
        return res == (ssize_t)len;
    }
    else {
        return fd;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 8) {
        fprintf(stderr,
                "usage: mkmanifest outfile seq_nr base_url file1 file2 vendor_id class_id [device_id]\n");
        exit(1);
    }

    size_t mlen = sizeof(suit_minimal_manifest_t) +
                  (2 * SUIT_MINIMAL_URL_MAXLEN);
    suit_minimal_manifest_t *m = calloc(1, mlen);
    if (!m) {
        fprintf(stderr, "malloc failed");
        exit(1);
    }

    m->manifest_version = SUIT_MINIMAL_MANIFEST_VERSION;
    m->seq_nr = strtol(argv[2], NULL, 10);

    char urls[2][SUIT_MINIMAL_URL_MAXLEN];
    for (int i = 0; i < 2; i++) {
        strcpy(urls[i], argv[3]);
        strcat(urls[i], argv[4 + i]);
    }

    uint8_t *urls_array[] = {
        (uint8_t *)urls[0], (uint8_t *)urls[1]
    };

    m->encoded_urls_len = suit_minimal_encode_urls(m->encoded_urls,
                                                   2 * SUIT_MINIMAL_URL_MAXLEN,
                                                   urls_array, 2);

    /* Generate UUID's following the instructions from
     * https://tools.ietf.org/html/draft-moran-suit-manifest-03#section-7.7.1
     */
    uuid_v5(&m->vendor_id, &uuid_namespace_dns, (uint8_t *)argv[6],
            strlen(argv[6]));

    uuid_v5(&m->class_id, &m->vendor_id, (uint8_t *)argv[7], strlen(argv[7]));

    suit_minimal_manifest_print(m);

    unlink(argv[1]);
    to_file(argv[1], m, sizeof(suit_minimal_manifest_t) + m->encoded_urls_len);
}

const char assert_crash_message[] = "FAILED ASSERTION.";
void core_panic(void)
{
    fprintf(stderr, "*** paniced.\n");
    exit(1);
}
