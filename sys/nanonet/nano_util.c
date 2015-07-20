#include <stdint.h>
#include <stddef.h>

/* calcsum - used to calculate IP and ICMP header checksums using
 * one's compliment of the one's compliment sum of 16 bit words of the header
 */
uint16_t nano_calcsum(uint16_t *buffer, size_t length)
{
    unsigned long sum;

    // initialize sum to zero and loop until length (in words) is 0
    for (sum=0; length>1; length-=2) // sizeof() returns number of bytes, we're interested in number of words
        sum += *buffer++;   // add 1 word of buffer to sum and proceed to the next

    // we may have an extra byte
    if (length==1)
        sum += (char)*buffer;

    sum = (sum >> 16) + (sum & 0xFFFF);  // add high 16 to low 16
    sum += (sum >> 16);          // add carry
    return ~sum;
}

void nano_util_addr_dump(uint8_t *addr, size_t len)
{
    for (int i = len; i >= 0; i--) {
        for (int j = 0; i < 2; j++) {
            char c = (char)((addr[i] >> (4 * j)) & 0x0f)
            if (c > 9) {
                c += 'a' - 10;
            }
            else {
                c += '9'
            }
            printf("%c", c);
        }
        if (i > 1) {
            printf(":");
        }
    }
}
