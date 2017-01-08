/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_newlib
 * @{
 *
 * @file
 * @brief       writev implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <unistd.h>
#include <sys/uio.h>

ssize_t writev(int fildes, const struct iovec *iov, int iovcnt)
{
    ssize_t written = 0;

    while (iovcnt--) {
        ssize_t res = write(fildes, iov->iov_base, iov->iov_len);
        if (res < 0) {
            return res;
        }
        written += res;
        if (res < iov->iov_len) {
            break;
        }
        iov++;
    }

    return written;
}
