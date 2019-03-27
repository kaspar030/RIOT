#include <stdio.h>
#include <stdlib.h>

#include "suit/minimal/manifest.h"

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
    if (argc != 6) {
        fprintf(stderr, "usage: mkmanifest seq_nr file1 file2 base_url outfile\n");
        exit(1);
    }

    size_t mlen = sizeof(suit_minimal_manifest_t);
    suit_minimal_manifest_t *m = calloc(1, mlen);
    if (!m) {
        fprintf(stderr, "malloc failed");
        exit(1);
    }

    m->seq_nr = strtol(argv[1], NULL, 10);

    size_t url_root_len = strlen(argv[4]);
    uint8_t *url_root = (uint8_t *)argv[4];

    for (unsigned slot = 0; slot < SUIT_MINIMAL_SLOT_NUMOF; slot++) {
        char *ptr = argv[2 + slot];
        size_t len = strlen(ptr);
        if ((url_root_len + len) > SUIT_MINIMAL_URL_MAXLEN) {
            fprintf(stderr, "filename of slot %u too long\n", slot);
            exit(1);
        }
        memcpy(m->slots[slot].url, url_root, url_root_len);
        memcpy(m->slots[slot].url + url_root_len, ptr, len);
        m->url_len[slot] = url_root_len + len;
    }

    suit_minimal_manifest_print(m);
    to_file(argv[5], m, sizeof(suit_minimal_manifest_t));
}
