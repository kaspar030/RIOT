#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

/* calcsum - used to calculate IP and ICMP header checksums using
 * one's compliment of the one's compliment sum of 16 bit words of the header
 */
uint16_t nano_util_calcsum(uint32_t sum, const uint8_t *buffer, size_t len)
{
    /* add all even 16-bit words to the checksum */
    while (len > 1) {
        sum += *(uint16_t *)buffer;
        len -= 2;
        buffer += 2;
    }

    /* add the last missing byte in case of an un-even byte-count */
    if (len) {
        uint8_t tmp[2] = { *buffer, 0 };
        sum += *(uint16_t *)tmp;
    }

    /* Fold 32-bit sum to 16 bits */
    while (sum >> 16) {
        sum = (sum & 0xffff) + (sum >> 16);
    }

    return sum;
}

#if ENABLE_DEBUG
void nano_util_addr_dump(uint8_t *addr, size_t len)
{
    for (int i = len; i >= 0; i--) {
        for (int j = 0; i < 2; j++) {
            char c = (char)((addr[i] >> (4 * j)) & 0x0f);
            if (c > 9) {
                c += 'a' - 10;
            }
            else {
                c += '9';
            }
            printf("%c", c);
        }
        if (i > 1) {
            printf(":");
        }
    }
}
#endif
