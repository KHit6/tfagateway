#pragma once

/*
 * Credentials to https://github.com/merbanan/rtl_433/blob/master/src/bit_util.c 
 * 
 * KHit6, 17.12.2024
 */

uint8_t lfsr_digest8_reflect(uint8_t const message[], int bytes, uint8_t gen, uint8_t key);
