#ifndef ERRNO_H
#define ERRNO_H

extern int errno;

enum {
    EAFNOSUPPORT = 1,
    EAGAIN,
    ENOTCONN,
    EADDRINUSE,
    EADDRNOTAVAIL,
    EPROTO,
    EBUSY,
    ENOSPC,
    EBADF,
    EBADMSG,
    EFAULT,
    EHOSTUNREACH,
    EINVAL,
    EMSGSIZE,
    ENETUNREACH,
    ENOBUFS,
    ENODEV,
    ENOENT,
    ENOMEM,
    ENOTSUP,
    EOPNOTSUPP,
    EOVERFLOW,
    ETIMEDOUT,
    ECANCELED,
    EIO,
    EEXIST,
};

#endif /* ERRNO_H */
