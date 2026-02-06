#pragma once

/*
 * Credentials to https://github.com/merbanan/rtl_433/blob/master/src/devices/tfa_30_3221.c 
 * and to https://github.com/emax73/TFA-Spring-sensor-30.3206.02/tree/master
 *
 * 
 * KHit6 26.01.2026
 * 
 */

class TFA_30_32XX {
public:
    TFA_30_32XX(uint8_t, uint64_t, uint8_t);
    
    uint8_t get_address(); 
    uint8_t get_checksum();
    uint8_t get_channel();
    float   get_temperature();
    float   get_humidity();
    bool    get_battery_state();
    uint8_t get_computed_checksum();

    bool    double_verify(void);
    
private:
    uint64_t  code_;
    uint8_t   size_;
    uint8_t   protocol_;
    bool      verified_;
};



