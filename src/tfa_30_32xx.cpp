/*
 * Credentials to https://github.com/merbanan/rtl_433/blob/master/src/devices/tfa_30_3221.c 
 * and to https://github.com/emax73/TFA-Spring-sensor-30.3206.02/tree/master
 *
 * KHit6, 26.01.2026
 */

/**
    Temperature/Humidity outdoor sensor TFA 30.3221.02.

    This is the same as LaCrosse-TX141THBv2 and should be merged.

    S.a. https://github.com/RFD-FHEM/RFFHEM/blob/master/FHEM/14_SD_WS.pm

        0    4    | 8    12   | 16   20   | 24   28   | 32   36
        --------- | --------- | --------- | --------- | ---------
        0000 1001 | 0001 0110 | 0001 0000 | 0000 0111 | 0100 1001
        IIII IIII | BSCC TTTT | TTTT TTTT | HHHH HHHH | XXXX XXXX
        1011 0001   1000 0010   1000 0101   0010 1000   0100 0011 0

    - I:  8 bit random id (changes on power-loss)
    - B:  1 bit battery indicator (0=>OK, 1=>LOW)
    - S:  1 bit sendmode (0=>auto, 1=>manual)
    - C:  2 bit channel valid channels are 0-2 (1-3)
    - T: 12 bit unsigned temperature, offset 500, scaled by 10
    - H:  8 bit relative humidity percentage
    - X:  8 bit checksum digest 0x31, 0xf4

    The sensor sends 3 repetitions at intervals of about 60 seconds.

    Caution: x.code contains the 4 sync pulses in advance 
*/

#include <cinttypes>
#include "tfa_30_32xx.h"
#include "bit_util.h"

const uint8_t OFFSET_FROM_RIGHT             = 1;                         
const uint8_t OFFSET_CHECKSUM_FROM_RIGHT    = (0 + OFFSET_FROM_RIGHT);
const uint8_t OFFSET_HUMIDITY_FROM_RIGHT    = (8 + OFFSET_FROM_RIGHT);  
const uint8_t OFFSET_TEMPERATURE_FROM_RIGHT = (16 + OFFSET_FROM_RIGHT);
const uint8_t OFFSET_CHANNEL_FROM_RIGHT     = (28 + OFFSET_FROM_RIGHT); 
const uint8_t OFFSET_SENDMODE_FROM_RIGHT    = (30 + OFFSET_FROM_RIGHT);
const uint8_t OFFSET_BATTERY_FROM_RIGHT     = (31 + OFFSET_FROM_RIGHT);
const uint8_t OFFSET_ADDRESS_FROM_RIGHT     = (32 + OFFSET_FROM_RIGHT);

TFA_30_32XX::TFA_30_32XX(uint8_t protocol, uint64_t code, uint8_t size) {
    // the original code consists of 41 bits, the lowest bit intention is unknown
    // with some sensors the lowest bit toggles, so mask it away
    this->code_     = code;
    this->size_     = size;
    this->protocol_ = protocol; 

    static uint64_t cached_code     = 0;
    static uint8_t  cached_size     = 0xff;
    static uint8_t  cached_protocol = 0xff;
    this->verified_                 = false;
    
    // expect at least 2 consecutive telegrams with the same content, same protocol and same size
    if ((cached_code == (code  & ~1LL)) && (cached_size == size) && (cached_protocol == protocol))
        this->verified_ = true;

    cached_code     = this->code_ & ~1LL;  
    cached_size     = this->size_;  
    cached_protocol = this->protocol_;  
}

uint8_t TFA_30_32XX::get_address() {
    return static_cast<uint8_t> ((this->code_ >> OFFSET_ADDRESS_FROM_RIGHT) & 0xff);;
}

uint8_t TFA_30_32XX::get_checksum() {
    return static_cast<uint8_t>((this->code_ >> OFFSET_CHECKSUM_FROM_RIGHT) & 0xff);
}

uint8_t TFA_30_32XX::get_channel() {
    return static_cast<uint8_t>((this->code_ >> OFFSET_CHANNEL_FROM_RIGHT) & 0x03);
}

float TFA_30_32XX::get_temperature() {
    return static_cast<float>(static_cast<int>((this->code_ >> OFFSET_TEMPERATURE_FROM_RIGHT) & 0xfff) - 500) / 10;
}

float TFA_30_32XX::get_humidity() {
    return static_cast<float>((this->code_ >> OFFSET_HUMIDITY_FROM_RIGHT) & 0xff);
}

bool TFA_30_32XX::get_battery_state() {
    return not static_cast<bool>((this->code_ >> OFFSET_BATTERY_FROM_RIGHT) & 1);
}

uint8_t TFA_30_32XX::get_computed_checksum() {
    uint8_t b[4];
    uint32_t t = uint32_t((this->code_ >> OFFSET_HUMIDITY_FROM_RIGHT) & 0xffffffff);
    
    b[0] = static_cast<uint8_t>(t >> 24);
    b[1] = static_cast<uint8_t>(t >> 16);
    b[2] = static_cast<uint8_t>(t >> 8);
    b[3] = static_cast<uint8_t>(t);

    return lfsr_digest8_reflect(b, 4, 0x31, 0xf4) & 0xFF;
}

bool TFA_30_32XX::double_verify(void) {
    return this->verified_;
}