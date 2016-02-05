#ifndef IPV6_RT_H
#define IPV6_RT_H

#include <stdint.h>

#include "kernel_types.h"
#include "net/ipv6/addr.h"

#define IPV6_ROUTING_TABLE_SIZE (8)
#define IPV6_RT_LIFETIME_NOEXPIRE ((uint32_t)-1)

typedef struct {
    int prefix_ref;
    uint8_t prefix_len;
    int next_hop_ref;
    kernel_pid_t iface;
    uint32_t lifetime;
    uint32_t flags;
} ipv6_rt_entry_t;

extern ipv6_rt_entry_t ipv6_routing_table[IPV6_ROUTING_TABLE_SIZE];

int ipv6_rt_put(const ipv6_addr_t *prefix, unsigned prefix_len, ipv6_addr_t *next_hop, kernel_pid_t iface, unsigned lifetime);
int ipv6_rt_del(const ipv6_addr_t *prefix, unsigned prefix_len);
int ipv6_rt_get_next_hop(ipv6_addr_t **next_hop, kernel_pid_t *via_iface, ipv6_addr_t *dst_addr);
void ipv6_rt_print(void);
void print_ipv6_addr(const ipv6_addr_t *addr);
void ipv6_rt_print_route(ipv6_rt_entry_t *entry);

#endif /* IPV6_RT_H */
