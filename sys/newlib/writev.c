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
