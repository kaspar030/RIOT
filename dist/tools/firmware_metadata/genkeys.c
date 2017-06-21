#include <stdio.h>

#include "common.h"
#include "tweetnacl.h"

const char genkeys_usage[] = "firmware genkeys <seckey-file> <pubkey-file>";
int genkeys(int argc, const char *argv[])
{
    unsigned char pk[crypto_sign_PUBLICKEYBYTES];
    unsigned char sk[crypto_sign_SECRETKEYBYTES];

    if (argc < 5) {
        fprintf(stderr, "usage: %s\n", genkeys_usage);
        return 1;
    }

    crypto_sign_keypair(pk,sk);

    const char err[] = "error writing to %s\n";
    if (!to_file(argv[3], sk, sizeof(sk))) {
        fprintf(stderr, err, argv[3]);
        return -1;
    }

    if (!to_file(argv[4], pk, sizeof(pk))) {
        fprintf(stderr, err, argv[4]);
        return -1;
    }

    return 0;
}
