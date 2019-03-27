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
        fprintf(stderr,
                "usage: mkmanifest seq_nr file1 file2 base_url outfile\n");
        exit(1);
    }

    size_t mlen = sizeof(suit_minimal_manifest_t) + (2 * SUIT_MINIMAL_URL_MAXLEN);
    suit_minimal_manifest_t *m = calloc(1, mlen);
    if (!m) {
        fprintf(stderr, "malloc failed");
        exit(1);
    }

    m->seq_nr = strtol(argv[1], NULL, 10);

    char urls[2][SUIT_MINIMAL_URL_MAXLEN];
    for (int i = 0; i < 2; i++) {
        strcpy(urls[i], argv[4]);
        strcat(urls[i], argv[2 + i]);
        printf("URL %i: \"%s\" %p\n", i, urls[i], (void*)urls[i]);
    }

    uint8_t *urls_array[] = {
        (uint8_t *)urls[0], (uint8_t *)urls[1]
    };


    m->encoded_urls_len = suit_minimal_encode_urls(m->encoded_urls,
                                                    2 * SUIT_MINIMAL_URL_MAXLEN,
                                                    urls_array, 2);

    suit_minimal_manifest_print(m);

    unlink(argv[5]);
    to_file(argv[5], m, sizeof(suit_minimal_manifest_t) + m->encoded_urls_len);
}
