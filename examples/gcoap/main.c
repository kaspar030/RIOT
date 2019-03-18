/*
 * Copyright (c) 2015-2016 Ken Bannister. All rights reserved.
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
 * @brief       gcoap example
 *
 * @author      Ken Bannister <kb2ma@runbox.com>
 *
 * @}
 */

#include <stdio.h>
#include "msg.h"

#include "net/gcoap.h"
#include "kernel_types.h"
#include "shell.h"

#define MAIN_QUEUE_SIZE (4)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern int gcoap_cli_cmd(int argc, char **argv);
extern void gcoap_cli_init(void);

#include "net/sock/util.h"
#include "net/nanocoap_sock.h"

static int _cb(void *arg, size_t offset, uint8_t *buf, size_t len, int more)
{
    printf("_cb: %p %u %p %u %i \"%.*s\"\n", arg, offset, buf, len, more, len, (char *)buf);
    return 0;
}

static int _nanocoap_get(int argc, char **argv)
{
    if (argc < 2) {
        return -EINVAL;
    }

    int res = nanocoap_get_blockwise_url(argv[1], COAP_BLOCKSIZE_32, _cb, NULL);
    if (res >= 0) {
        puts("success");
    }
    else {
        printf("res=%i\n", res);
    }

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "coap", "CoAP example", gcoap_cli_cmd },
    { "nanocoap", "nanocoap test", _nanocoap_get },
    { NULL, NULL, NULL }
};

int main(void)
{
    /* for the thread running the shell */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    gcoap_cli_init();
    puts("gcoap example app");

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should never be reached */
    return 0;
}
