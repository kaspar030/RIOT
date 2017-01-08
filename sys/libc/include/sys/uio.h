/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  posix
 * @{
 */

/**
 * @file
 * @brief   libc header for scatter/gather I/O
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */
#ifndef UIO_H
#define UIO_H

#include <stdlib.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Structure for scatter/gather I/O.
 */
struct iovec {
    void *iov_base;     /**< Pointer to data.   */
    size_t iov_len;     /**< Length of data.    */
};

/**
 * @brief   Write a struct iovec to file descriptor
 *
 * @param[in]   fildes  file descriptor to write to
 * @param[in]   iov     ptr to iovec array to read from
 * @param[in]   iovcnt  number of entries in @p iov
 *
 * @returns     number of bytes actually written
 */
ssize_t writev(int fildes, const struct iovec *iov, int iovcnt);

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* UIO_H */
