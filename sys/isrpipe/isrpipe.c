#include "isrpipe.h"

void isrpipe_init(isrpipe_t *isrpipe, char *buf, size_t bufsize)
{
    mutex_init(&isrpipe->mutex);
    tsrb_init(&isrpipe->tsrb, buf, bufsize);
}

int isrpipe_write_one(isrpipe_t *isrpipe, char c)
{
    int res = tsrb_add_one(&isrpipe->tsrb, c);

    /* `res` is either 0 on success or -1 when the buffer is full. Either way,
     * unlocking the mutex is fine.
     */
    mutex_unlock(&isrpipe->mutex);

    return res;
}

int isrpipe_read(isrpipe_t *isrpipe, char *buffer, size_t count)
{
    int res;

    while (!(res = tsrb_get(&isrpipe->tsrb, buffer, count))) {
        mutex_lock(&isrpipe->mutex);
    }
    return res;
}
