#ifndef NANO_ROUTE_H
#define NANO_ROUTE_H

typedef struct ipv4_route {
    uint32_t route;
    uint32_t netmask;
    uint32_t next_hop;
    nano_dev_t *dev;
} ipv4_route_t;

typedef struct ipv6_route {
    uint8_t route[16];
    unsigned prefix_len;
    uint8_t next_hop[16];
    nano_dev_t *dev;
} ipv6_route_t;

extern ipv4_route_t ipv4_routes[];
extern ipv6_route_t ipv6_routes[];

#endif /* NANO_ROUTE_H */
