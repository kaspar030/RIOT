#ifndef SOCK_TYPES_H
#define SOCK_TYPES_H

/**
 * @brief   Raw IP sock type
 * @internal
 */
struct sock_ip {
    sock_ip_ep_t local;                 /**< local end-point */
    sock_ip_ep_t remote;                /**< remote end-point */
    uint16_t flags;                     /**< option flags */
};

/**
 * @brief   UDP sock type
 * @internal
 */
struct sock_udp {
    sock_udp_ep_t local;                /**< local end-point */
    sock_udp_ep_t remote;               /**< remote end-point */
    uint16_t flags;                     /**< option flags */
};


#endif /* SOCK_TYPES_H */
