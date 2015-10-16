#ifndef ERRNO_H
#define ERRNO_H

extern int errno;

enum {
    EBADF,
    EINVAL,
    EOVERFLOW,
    ETIMEDOUT,
    ENOTSUP,
    ENODEV,
    ENOMEM,
    ENOENT,
    EBADMSG,
    ENOBUFS,
    EAFNOSUPPORT,
    EHOSTUNREACH,
    EMSGSIZE,
};

#endif /* ERRNO_H */
