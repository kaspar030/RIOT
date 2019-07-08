/*
 * Copyright (C) 2018 Freie Universität Berlin
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup    tests
 * @{
 *
 * @file
 * @brief      Tests for pkg libcose
 *
 * @author     Koen Zandberg <koen@bergzand.net>
 */

/* Number of nodes allocated for cn-cbor */
#define MAX_NUMBER_BLOCKS 32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cose.h"
#include "cose/sign.h"
#include "cose/crypto.h"
#include "embUnit.h"
#include "memarray.h"
#include "random.h"

/* Example payload */
static char payload[] = "Input string";
/* Key ID's */
static const char kid[] = "peter@riot-os.org";
static const char kid2[] = "schmerzl@riot-os.org";
/* Key pairs */
static unsigned char pk[COSE_CRYPTO_SIGN_ED25519_PUBLICKEYBYTES];
static unsigned char sk[COSE_CRYPTO_SIGN_ED25519_SECRETKEYBYTES];
static unsigned char pk2[COSE_CRYPTO_SIGN_ED25519_PUBLICKEYBYTES];
static unsigned char sk2[COSE_CRYPTO_SIGN_ED25519_SECRETKEYBYTES];
static unsigned char symmkey[COSE_CRYPTO_AEAD_CHACHA20POLY1305_KEYBYTES];
/* static uint8_t nonce[COSE_CRYPTO_AEAD_CHACHA20POLY1305_NONCEBYTES] = { 0 }; */
/* COSE structs */
static cose_sign_enc_t sign;
static cose_sign_dec_t verify;
static cose_signature_t signature;
static cose_key_t signer, signer2, symm;
static cose_encrypt_t test_encrypt, test_decrypt;
/* COSE sign buffer */
static uint8_t buf[1024];
/*Signature Verification buffer */
static uint8_t vbuf[1024];

static void setUp(void)
{
    /* Initialize */
    random_init(0);
    /* Clear buffer */
    memset(buf, 0, sizeof(buf));
    memset(vbuf, 0, sizeof(vbuf));
}

static void test_libcose_01(void)
{
    /* Set up first signer */
    cose_key_init(&signer);
    cose_key_set_keys(&signer, COSE_EC_CURVE_ED25519, COSE_ALGO_EDDSA, pk, NULL,
                      sk);
    cose_crypto_keypair_ed25519(&signer);
    cose_key_set_kid(&signer, (uint8_t *)kid, sizeof(kid) - 1);

    /* Initialize struct */
    cose_sign_init(&sign, COSE_FLAGS_UNTAGGED);
    cose_signature_init(&signature);

    /* Add payload */
    cose_sign_set_payload(&sign, payload, sizeof(payload));

    /* First signer */
    cose_sign_add_signer(&sign, &signature, &signer);

    /* Encode COSE sign object */
    uint8_t *out = NULL;
    ssize_t encode_size = cose_sign_encode(&sign, buf, sizeof(buf), &out);
    TEST_ASSERT(encode_size > 0);
    /* Decode again */
    TEST_ASSERT_EQUAL_INT(cose_sign_decode(&verify, out, encode_size), 0);
    /* Verify with signature slot 0 */
    cose_signature_dec_t vsignature;
    cose_sign_signature_iter_init(&vsignature);
    TEST_ASSERT(cose_sign_signature_iter(&verify, &vsignature));
    int verification = cose_sign_verify(&verify, &vsignature, &signer, vbuf,
                                        sizeof(vbuf));
    TEST_ASSERT_EQUAL_INT(0, verification);
    /* Modify payload */
    ((int *)(verify.payload))[0]++;
    verification =
        cose_sign_verify(&verify, &vsignature, &signer, vbuf, sizeof(vbuf));
    /* Should fail due to modified payload */
    TEST_ASSERT_EQUAL_INT(-2, verification);
}

static void test_libcose_02(void)
{
    /* Set up first signer */
    cose_key_init(&signer);
    cose_key_set_keys(&signer, COSE_EC_CURVE_ED25519, COSE_ALGO_EDDSA, pk, NULL,
                      sk);
    cose_crypto_keypair_ed25519(&signer);
    cose_key_set_kid(&signer, (uint8_t *)kid, sizeof(kid) - 1);

    /* Second signer */
    cose_key_init(&signer2);
    cose_key_set_keys(&signer2, COSE_EC_CURVE_ED25519, COSE_ALGO_EDDSA, pk2,
                      NULL, sk2);
    cose_crypto_keypair_ed25519(&signer2);
    cose_key_set_kid(&signer2, (uint8_t *)kid2, sizeof(kid2) - 1);

    /* Initialize structs */
    cose_sign_init(&sign, 0);
    cose_signature_init(&signature);

    /* Add payload */
    cose_sign_set_payload(&sign, payload, sizeof(payload));

    /* Signers */
    cose_sign_add_signer(&sign, &signature, &signer);
    cose_sign_add_signer(&sign, &signature, &signer2);

    uint8_t *out = NULL;
    size_t len = cose_sign_encode(&sign, buf, sizeof(buf), &out);

    TEST_ASSERT(len > 0);
    TEST_ASSERT_EQUAL_INT(cose_sign_decode(&verify, out, len), 0);

    /* Test correct signature with correct signer */
    cose_signature_dec_t vsignature;

    cose_sign_signature_iter_init(&vsignature);
    TEST_ASSERT(cose_sign_signature_iter(&verify, &vsignature));
    TEST_ASSERT_EQUAL_INT(cose_sign_verify(&verify, &vsignature, &signer, vbuf,
                                           sizeof(vbuf)), 0);
    TEST_ASSERT(cose_sign_verify(&verify, &vsignature, &signer2, vbuf,
                                 sizeof(vbuf)) != 0);

    TEST_ASSERT(cose_sign_signature_iter(&verify, &vsignature));

    TEST_ASSERT(cose_sign_verify(&verify, &vsignature, &signer, vbuf,
                                 sizeof(vbuf)) != 0);

    TEST_ASSERT_EQUAL_INT(cose_sign_verify(&verify, &vsignature, &signer2, vbuf,
                                           sizeof(vbuf)), 0);
}

static void test_libcose_03(void)
{
    cose_key_init(&symm);
    cose_encrypt_init(&test_encrypt, 0);
    cose_encrypt_init(&test_decrypt, 0);

    cose_crypto_keygen(symmkey, sizeof(symmkey), COSE_ALGO_CHACHA20POLY1305);
    cose_key_set_kid(&symm, (uint8_t *)kid, sizeof(kid) - 1);
    cose_key_set_keys(&symm, 0, COSE_ALGO_CHACHA20POLY1305, NULL, NULL,
                      symmkey);
    cose_encrypt_add_recipient(&test_encrypt, &symm);
    cose_encrypt_set_algo(&test_encrypt, COSE_ALGO_DIRECT);

    cose_encrypt_set_payload(&test_encrypt, payload, sizeof(payload) - 1);

    /* uint8_t *out = NULL; */
    /* ssize_t len = cose_encrypt_encode(&test_encrypt, buf, sizeof(buf), nonce, */
    /*                                   &out, &ct); */
    /* TEST_ASSERT(len > 0); */
    /* TEST_ASSERT_EQUAL_INT(cose_encrypt_decode(&test_decrypt, out, len, &ct), 0); */
    /* size_t plaintext_len = 0; */
    /* int res = cose_encrypt_decrypt(&test_decrypt, &symm, 0, buf, sizeof(buf), */
    /*                                vbuf, &plaintext_len, &ct); */
    /* TEST_ASSERT_EQUAL_INT(res, 0); */
    /* TEST_ASSERT_EQUAL_INT(plaintext_len, sizeof(payload) - 1); */
}

Test *tests_libcose(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_libcose_01), new_TestFixture(test_libcose_02),
        new_TestFixture(test_libcose_03),
    };

    EMB_UNIT_TESTCALLER(libcose_tests, setUp, NULL, fixtures);
    return (Test *)&libcose_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_libcose());
    TESTS_END();
    return 0;
}
