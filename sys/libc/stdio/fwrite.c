#include <stdio.h>

size_t fwrite(const void *ptr, size_t size, size_t nmemb,
              FILE *stream)
{
    size_t written = 0;
    while (nmemb--) {
        written += stream->out(stream, ptr, size);
        ptr += size;
    }
    return written;
}
