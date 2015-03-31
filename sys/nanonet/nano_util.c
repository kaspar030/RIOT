#include <stdint.h>

/* calcsum - used to calculate IP and ICMP header checksums using
 * one's compliment of the one's compliment sum of 16 bit words of the header
 */
uint16_t nano_calcsum(uint16_t *buffer, int length)
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

