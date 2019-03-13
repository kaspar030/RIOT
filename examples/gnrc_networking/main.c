/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 * @brief       Example application for demonstrating the RIOT network stack
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "msg.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern int udp_cmd(int argc, char **argv);

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

    char hostport[SOCK_HOSTPORT_MAXLEN];
    char urlpath[SOCK_URLPATH_MAXLEN];
    sock_udp_ep_t remote;

    if (sock_urlsplit(argv[1], hostport, urlpath) < 0) {
        return -EINVAL;
    }

    if (sock_udp_str2ep(&remote, hostport) < 0) {
        return -EINVAL;
    }

    if (!remote.port) {
        remote.port = COAP_PORT;
    }

    nanocoap_get_blockwise(&remote, urlpath, COAP_BLOCKSIZE_32B, _cb, NULL);

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "get", "coap test", _nanocoap_get },
    { "udp", "send data over UDP and listen on UDP ports", udp_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT network stack example application");

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
