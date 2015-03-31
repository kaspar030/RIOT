#ifndef NANO_ROUTE_H
#define NANO_ROUTE_H

typedef struct ipv4_route {
    uint32_t route;
    uint32_t netmask;
    nano_dev_t *dev;
} ipv4_route_t;

extern ipv4_route_t ipv4_routes[];

#endif /* NANO_ROUTE_H */
