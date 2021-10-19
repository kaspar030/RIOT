#ifndef ERRNO_H
#define ERRNO_H

extern int errno;

enum {
    _ERRNO_START = 0,
    EADDRINUSE,
    EADDRNOTAVAIL,
    EAFNOSUPPORT,
    EAGAIN,
    EALREADY,
    EBADF,
    EBADMSG,
    EBUSY,
    ECANCELED,
    EEXIST,
    EFAULT,
    EHOSTUNREACH,
    EINVAL,
    EIO,
    EMSGSIZE,
    ENETUNREACH,
    ENOBUFS,
    ENODEV,
    ENOENT,
    ENOMEM,
    ENOSPC,
    ENOTCONN,
    ENOTSUP,
    ENXIO,
    EOPNOTSUPP,
    EOVERFLOW,
    EPROTO,
    ERANGE,
    ETIMEDOUT,
};

#endif /* ERRNO_H */
