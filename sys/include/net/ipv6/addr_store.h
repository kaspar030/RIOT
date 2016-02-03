#ifndef ADDR_STORE_H
#define ADDR_STORE_H

#define IPV6_ADDRSTORE_SIZE (16)

typedef struct {
    ipv6_addr_t addr;
    unsigned refcount;
} ipv6_addrstore_entry_t;

int ipv6_addrstore_add(const ipv6_addr_t *addr);
void ipv6_addrstore_del(const ipv6_addr_t *addr);

extern ipv6_addrstore_entry_t ipv6_addrstore[IPV6_ADDRSTORE_SIZE];

static inline ipv6_addr_t* ipv6_addrstore_get(int entry)
{
    return (entry == -1) ? NULL : &ipv6_addrstore[entry].addr;
}

static inline void ipv6_addrstore_get_copy(ipv6_addr_t *tgt, int entry)
{
    *tgt = ipv6_addrstore[entry].addr;
}

static inline void ipv6_addrstore_unref(int entry)
{
    ipv6_addrstore[entry].refcount--;
}

#endif /* ADDR_STORE_H */
