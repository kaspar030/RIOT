#include <assert.h>

#include "net/ipv6/addr.h"
#include "net/ipv6/addr_store.h"

ipv6_addrstore_entry_t ipv6_addrstore[IPV6_ADDRSTORE_SIZE];

int _find(const ipv6_addr_t *addr)
{
    int first_free = -1;
    ipv6_addrstore_entry_t* pos = ipv6_addrstore;

    for (int i = 0; i < IPV6_ADDRSTORE_SIZE; i++, pos++) {
        if (ipv6_addr_equal(addr, &pos->addr)) {
            return i;
        } else {
            if ((first_free == -1) && (!pos->refcount)) {
                first_free = i;
            }
        }
    }

    return first_free;
}

int ipv6_addrstore_add(const ipv6_addr_t *addr)
{
    int pos = _find(addr);
    if (pos >= 0) {
        ipv6_addrstore_entry_t* entry = &ipv6_addrstore[pos];
        if (!entry->refcount) {
            entry->addr = *addr;
        }
        entry->refcount++;
    }
    return pos;
}

void ipv6_addrstore_del(const ipv6_addr_t *addr)
{
    int pos = _find(addr);
    if (pos >= 0) {
        assert(ipv6_addrstore[pos].refcount);
        ipv6_addrstore[pos].refcount--;
    }
}
