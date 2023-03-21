/*

man byteorder

byte order:
    big-endian:    0x12345678
    little-endian: 0x78563412
    big-endian:    0x12345678 0x78563412

    the standard byte order is big-endian.

    sulotion:
        1. use htonl() and ntohl() to convert
        2. use htons() and ntohs() to convert
        3. use memcpy() to convert

*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>


void send_data(uint8_t *data, uint32_t len)
{
    uint32_t i;
    for (i = 0; i < len; i++) {
        printf("%02hhx ", data[i]);
    }
    putchar('\n');
}

int main(void)
{
    int x = 0x12345678;

    uint8_t buffer[256];

    memcpy(buffer, &x, sizeof(x));

    send_data(buffer, sizeof(x));

    return 0;
}