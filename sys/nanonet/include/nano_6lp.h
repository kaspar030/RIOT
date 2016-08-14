#ifndef NANO_6LP_H
#define NANO_6LP_H

#include <stdint.h>
#include <string.h>

#include "nano_ctx.h"
#include "nano_sndbuf.h"
#include "nano_ipv6.h"

#define IPV6_NEXTHDR_UDP        (17U)
#define IPV6_NEXTHDR_ICMP       (58U)

int nano_6lp_handle(nano_ctx_t *ctx, size_t offset);
int nano_6lp_reply(nano_ctx_t *ctx);

#endif /* NANO_6LP_H */
