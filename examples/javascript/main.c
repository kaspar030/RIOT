/*
 * Copyright (C) 2017 Inria
 *               2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example of how to use javascript on RIOT
 *
 * @author      Emmanuel Baccelli <emmanuel.baccelli@inria.fr>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "msg.h"
#include "xtimer.h"
#include "lwm2m.h"
#include "js.h"

/* include headers generated from *.js */
#include "lib.js.h"
#include "local.js.h"
#include "ps.h"
#include "hashes/sha256.h"
#include "tweetnacl.h"

static const unsigned char pk[] = {
  0xf4, 0x53, 0x44, 0x70, 0x3e, 0x71, 0x84, 0xe2, 0x8f, 0x75, 0x4d, 0x72,
  0x71, 0xb4, 0x3a, 0xd1, 0x9e, 0x45, 0x10, 0x5f, 0x71, 0xc3, 0x85, 0x59,
  0x20, 0xe3, 0x84, 0x6a, 0x21, 0x60, 0x3c, 0xe2
};

static event_queue_t event_queue;

char script[512];

#define MAIN_QUEUE_SIZE (4)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

/* import "ifconfig" shell command, used for printing addresses */
extern int _netif_config(int argc, char **argv);

void js_start(event_t *unused)
{
    (void)unused;

    size_t script_len = strlen(script);
    if (script_len) {
        puts("Initializing jerryscript engine...");
        js_init();

        puts("Executing lib.js...");
        js_run(lib_js, lib_js_len);

        puts("Executing local.js...");
        js_run(local_js, local_js_len);

        puts("Executing script...");
        js_run((jerry_char_t*)script, script_len);
    }
    else {
        puts("Emtpy script, not executing.");
    }
}

static event_t js_start_event = { .handler=js_start };

void js_restart(void)
{
    js_shutdown(&js_start_event);
}

typedef struct {
    event_t super;
    unsigned length;
} check_script_event_t;

void check_script_handler(event_t *event);
static check_script_event_t _check_script_event = { .super.handler = check_script_handler };

void check_script(unsigned length)
{
    _check_script_event.length = length;
    event_post_first(&event_queue, (event_t *)&_check_script_event);
}

void check_script_handler(event_t *event)
{
    check_script_event_t *check_script_event = (check_script_event_t *)event;

    unsigned length = check_script_event->length;

    puts("checking received script...");

    uint8_t sm[crypto_sign_BYTES + SHA256_DIGEST_LENGTH];
    uint8_t m[crypto_sign_BYTES + SHA256_DIGEST_LENGTH];
    if (length < (crypto_sign_BYTES + 1)) {
        puts("received script too short (no sig?)");
        *script = '\0';
        return;
    }

    memcpy(sm, script + length - crypto_sign_BYTES, crypto_sign_BYTES);
    script[length - crypto_sign_BYTES] = '\0';

    sha256_context_t sha256;
    sha256_init(&sha256);
    sha256_update(&sha256, script, length - crypto_sign_BYTES);
    sha256_final(&sha256, sm + crypto_sign_BYTES);

    puts("verifying script...");
    unsigned long long mlen;
    if (crypto_sign_open(m, &mlen, sm, sizeof(sm), pk)) {
        puts("script verification failed");
        *script='\0';
    }
    else {
        puts("script received:");
        puts("-----");
        puts(script);
        puts("-----");
        puts("restarting js.");
        js_restart();
    }
}

int main(void)
{
    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    puts("waiting for network config");
    xtimer_sleep(3);

    /* print network addresses */
    puts("Configured network interfaces:");
    _netif_config(0, NULL);

    /* register to LWM2M server */
    puts("initializing coap, registering at lwm2m server");
    lwm2m_init();
    lwm2m_register();

    puts("setting up event queue");
    event_queue_init(&event_queue);
    js_event_queue = &event_queue;

    ps();
    puts("Entering event loop...");
    event_loop(&event_queue);

    return 0;
}
