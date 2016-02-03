#include <stdio.h>

#include "net/ipv6/rt.h"
#include "net/ipv6/addr_store.h"
#include "xtimer.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

#if ENABLE_DEBUG

#include "fmt.h"
#include "div.h"
void print_ipv6_addr(const ipv6_addr_t *addr)
{
    if (!addr) {
        print_str("(null)");
    } else {
        char addr_str[IPV6_ADDR_MAX_STR_LEN];
        ipv6_addr_to_str(addr_str, addr, sizeof(addr_str));
        print_str(addr_str);
    }
}
#endif

ipv6_rt_entry_t ipv6_routing_table[IPV6_ROUTING_TABLE_SIZE];

enum {
    RT_FLAG_ACTIVE = 1
};

static int _expired(ipv6_rt_entry_t *entry)
{
    if (entry->lifetime == IPV6_RT_LIFETIME_NOEXPIRE) {
        return 0;
    }
    else {
        return (xtimer_now() > entry->lifetime);
    }
}

/* returns entry with matching prefix and prefix length or unused entry */
static int _find(ipv6_addr_t *prefix, int prefix_len)
{
    int res = -1;
    ipv6_rt_entry_t *entry = ipv6_routing_table;

    for (int i = 0; i < IPV6_ROUTING_TABLE_SIZE; i++, entry++) {
        if (entry->flags) {
            if ((entry->prefix_len == prefix_len)
                    && ipv6_addr_equal(prefix, ipv6_addrstore_get(entry->prefix_ref))) {
                res = i;
                break;
            }
        }
        else if (res == -1) {
            res = i;
        }
    }

    return res;
}

int ipv6_rt_put(ipv6_addr_t *prefix, int prefix_len, ipv6_addr_t *next_hop, kernel_pid_t iface, unsigned lifetime)
{
    int pos = _find(prefix, prefix_len);

    if (pos >= 0) {
        ipv6_rt_entry_t *entry = &ipv6_routing_table[pos];
        if (!entry->flags) {
            /* _find returned an unused entry */
            entry->prefix_ref = ipv6_addrstore_add(prefix);
            entry->prefix_len = prefix_len;
            entry->flags |= RT_FLAG_ACTIVE;
        } else {
            ipv6_addrstore_unref(entry->next_hop_ref);
        }

        entry->next_hop_ref = next_hop ? ipv6_addrstore_add(next_hop) : -1;
        entry->iface = iface;
        if (lifetime == IPV6_RT_LIFETIME_NOEXPIRE) {
            entry->lifetime = lifetime;
        }
        else {
            entry->lifetime = xtimer_now() + (lifetime*1000);
        }
    }
    return pos;
}

int ipv6_rt_del(ipv6_addr_t *prefix, int prefix_len)
{
    int pos = _find(prefix, prefix_len);

    if (pos >= 0) {
        ipv6_rt_entry_t *entry = &ipv6_routing_table[pos];
        if (entry->flags & RT_FLAG_ACTIVE) {
            entry->flags = 0;
            ipv6_addrstore_unref(entry->prefix_ref);
            ipv6_addrstore_unref(entry->next_hop_ref);
            return 0;
        }
    }
    return pos;
}

int ipv6_rt_get_next_hop(ipv6_addr_t **next_hop, kernel_pid_t *via_iface, ipv6_addr_t *dst_addr)
{
    int best_match_len = 0;
    ipv6_rt_entry_t *best_match = NULL;
    ipv6_rt_entry_t *entry = ipv6_routing_table;

#if ENABLE_DEBUG
    print_str("ipv6_rt_get_next_hop(): getting next hop for ");
    print_ipv6_addr(dst_addr);
    print_str("\n");
#endif

    for (int i = 0; i < IPV6_ROUTING_TABLE_SIZE; i++, entry++) {
        /* skip unused or expired entries */
        if (!entry->flags || _expired(entry)) {
            continue;
        }
        ipv6_addr_t* prefix = ipv6_addrstore_get(entry->prefix_ref);

        int match = ipv6_addr_match_prefix(dst_addr, prefix);

        /* no match -> continue (but not for default route)*/
        if (!match && entry->prefix_len) {
            continue;
        }

        if (match < entry->prefix_len) {
            continue;
        }

        /* update best match so far */
        if (match > best_match_len) {
            best_match_len = match;
            best_match = entry;
        }

        /* found perfect match, so bail out */
        if (match == 128) {
            break;
        }
    }

    if (best_match) {
        *next_hop = ipv6_addrstore_get(best_match->next_hop_ref);
        if (!*next_hop) {
            *next_hop = dst_addr;
        }
        *via_iface = best_match->iface;

#if ENABLE_DEBUG
        print_str("ipv6_rt_get_next_hop(): next hop is ");
        print_ipv6_addr(*next_hop);
        print_str(" over interface ");
        print_u32_dec(*via_iface);
        print_str("\n");
#endif
        return 0;
    }

#if ENABLE_DEBUG
    print_str("ipv6_rt_get_next_hop(): no hop found\n");
#endif

    return -1;
}

void ipv6_rt_print(void)
{
    ipv6_rt_entry_t *entry = ipv6_routing_table;

    for (int i = 0; i < IPV6_ROUTING_TABLE_SIZE; i++, entry++) {
        if (!entry->flags) {
            continue;
        }

        print_ipv6_addr(ipv6_addrstore_get(entry->prefix_ref));
        if (entry->prefix_len) {
            printf("/%i", entry->prefix_len);
        }

        if (entry->next_hop_ref != -1) {
            print_str(" via ");
            print_ipv6_addr(ipv6_addrstore_get(entry->next_hop_ref));
        }
        printf(" dev %"PRIkernel_pid" flags 0x%08x", entry->iface, entry->flags);
        if (entry->lifetime != IPV6_RT_LIFETIME_NOEXPIRE) {
            printf(" expires %us\n", (entry->lifetime - xtimer_now())/SEC_IN_USEC);
        }
        if (_expired(entry)) {
            print_str(" (expired)\n");
        }
        else {
            print_str("\n");
        }
    }
}
