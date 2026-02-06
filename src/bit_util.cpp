
/*
 * Credentials to https://github.com/merbanan/rtl_433/blob/master/src/bit_util.c 
 * 
 * KHit6, 17.12.2024
 */
 
#include <cinttypes>
#include "bit_util.h"

uint8_t lfsr_digest8_reflect(uint8_t const message[], int bytes, uint8_t gen, uint8_t key) {
    uint8_t sum = 0;
    // Process message from last byte to first byte (reflected)
    for (int k = bytes - 1; k >= 0; --k) {
        uint8_t data = message[k];
        // Process individual bits of each byte (reflected)
        for (int i = 0; i < 8; ++i) {
            // XOR key into sum if data bit is set
            if ((data >> i) & 1) {
                sum ^= key;
            }

            // roll the key left (actually the msb is dropped here)
            // and apply the gen (needs to include the dropped msb as lsb)
            if (key & 0x80)
                key = (key << 1) ^ gen;
            else
                key = (key << 1);
        }
    }
    return sum;
} 
