#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

/* calcsum - used to calculate IP and ICMP header checksums using
 * one's compliment of the one's compliment sum of 16 bit words of the header
 */
uint16_t nano_util_calcsum(uint16_t csum, const uint8_t *buffer, size_t len)
{
    uint32_t sum = csum;
    uint16_t *data = (uint16_t *)buffer;

    /* add all even 16-bit words to the checksum */
    for (size_t i = 0; i < (len / 2); i++) {
        sum += data[i];
    }
    /* add the last missing byte in case of an un-even byte-count */
    if (len & 0x01) {
        sum += (buffer[len - 1] << 8);
    }
    /* add the upper 16 bit to the lower 16 bit and add the eventual carry */
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (uint16_t)sum;
}

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
