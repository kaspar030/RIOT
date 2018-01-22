/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License v2. See the file LICENSE for more details.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "firmware.h"
#include "common.h"
#include "tweetnacl.h"

const char sign_usage[] = "firmware sign <file> <seckeyfile>";


static int _sign(const unsigned char *data, unsigned len, const unsigned char *sk, unsigned char *out)
{
    unsigned char sm[len + crypto_sign_BYTES];
    unsigned long long smlen;
    crypto_sign(sm, &smlen, data, len, sk);
    memcpy(out, sm, crypto_sign_BYTES);
    return 0;
}

int sign(int argc, char *argv[])
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    unsigned char sk[FIRMWARE_SECKEY_LEN];
    unsigned char sig[crypto_sign_BYTES];

    if (argc < 3) {
        goto usage;
    }

    if (do_sha256(argv[1], hash, 0) <= 0) {
        fprintf(stderr, "error hashing input file\n");
        return 1;
    }

    if (!from_file(argv[2], sk, sizeof(sk))) {
        fprintf(stderr, "error reading keyfile\n");
        return 1;
    }

    _sign(hash, sizeof(hash), sk, sig);

    if (argc == 4) {
        to_file(argv[3], sig, sizeof(sig));
    }
    else {
        for (unsigned i = 0; i < crypto_sign_BYTES; i++) {
            printf("%02x", sig[i] & 0xff);
        }
        puts("");
    }

    return 0;

usage:
    fprintf(stderr, "usage: %s\n", sign_usage);
    return 1;
}
