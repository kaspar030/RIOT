/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       Unittests for ztimer_extend
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#include "ztimer.h"
#include "ztimer/mock.h"
#include "ztimer/extend.h"

#include "embUnit/embUnit.h"

#include "tests-ztimer.h"

/**
 * @brief   Simple callback for counting alarms
 */
static void cb_incr(void *arg)
{
    uint32_t *ptr = arg;
    *ptr += 1;
}

/**
 * @brief   Testing the counter of a 32 bit extended 8 bit counter
 */
static void test_ztimer_extend_now_rollover(void)
{
    ztimer_mock_t zmock;
    ztimer_extend_t zx;
    ztimer_dev_t *z = &zx.super;
    ztimer_dev_t *zm = &zmock.super;

    ztimer_mock_init(&zmock, 8);
    ztimer_extend_init(&zx, &zmock.super, 8);
    uint32_t now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(0, now);
    ztimer_mock_advance(&zmock, 50);
    now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(50, now);
    ztimer_mock_advance(&zmock, 50);
    now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(100, now);
    ztimer_mock_advance(&zmock, 50);
    now = ztimer_now(zm);
    TEST_ASSERT_EQUAL_INT(150, now);
    now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(150, now);
    ztimer_mock_advance(&zmock, 50);
    now = ztimer_now(zm);
    TEST_ASSERT_EQUAL_INT(200, now);
    now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(200, now);
    ztimer_mock_advance(&zmock, 50);
    now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(250, now);
    ztimer_mock_advance(&zmock, 50); /* -> rollover in lower clock */
    now = ztimer_now(zm);
    TEST_ASSERT_EQUAL_INT((300 - (1 << 8)), now);
    now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(300, now);
    ztimer_mock_advance(&zmock, 50);
    now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(350, now);
}

/**
 * @brief   Testing long alarms on an 32 bit extended 8 bit clock
 */
static void test_ztimer_extend_set_long(void)
{
    ztimer_mock_t zmock;
    ztimer_extend_t zx;
    ztimer_dev_t *z = &zx.super;

    ztimer_mock_init(&zmock, 8);
    ztimer_extend_init(&zx, &zmock.super, 8);
    uint32_t now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(0, now);
    uint32_t counts[10] = {0};
    static const unsigned nalarms = sizeof(counts) / sizeof(counts[0]);
    ztimer_t alarms[nalarms];
    for (unsigned k = 0; k < nalarms; ++k) {
        alarms[k] = (ztimer_t){ .callback = cb_incr, .arg = &counts[k], };
        ztimer_set(z, &alarms[k], k * 1000);
    };
    for (uint32_t t = 50; t <= nalarms * 1000; t += 50) {
        ztimer_mock_advance(&zmock, 50);
        for (unsigned k = 0; k < nalarms; ++k) {
            if (t >= k * 1000) {
                TEST_ASSERT_EQUAL_INT(1, counts[k]);
            }
            else {
                TEST_ASSERT_EQUAL_INT(0, counts[k]);
            }
        }
    }
    TEST_ASSERT_EQUAL_INT(1, counts[nalarms - 1]);
    now = ztimer_now(z);
    TEST_ASSERT_EQUAL_INT(nalarms * 1000, now);
}

Test *tests_ztimer_extend_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_ztimer_extend_now_rollover),
        new_TestFixture(test_ztimer_extend_set_long),
    };

    EMB_UNIT_TESTCALLER(ztimer_tests, NULL, NULL, fixtures);

    return (Test *)&ztimer_tests;
}

/** @} */
