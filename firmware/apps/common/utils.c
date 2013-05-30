/***
 *
 * Date: 2010-04-30
 * Author: Ivan A-R <ivan@tuxotronic.org>
 *
 */

#include "utils.h"

/*
 * Some functions
 */

const uint32_t power10[POWER10_SIZE] = {
    1000000000,
    100000000,
    10000000,
    1000000,
    100000,
    10000,
    1000,
    100,
    10,
};

int numericLength(int32_t v)
{
    int lng = 1;
    if(v < 0)
    {
        lng++;
        v = -v;
    }
    int i;
    for(i=(sizeof(power10)/sizeof(power10[0])-1); i>=0; i--)
    {
        if((uint32_t)v < power10[i]) break;
        lng++;
    }
    return lng;
}

char* uitoa(uint32_t val, char* dest)
{
    uint32_t ipow;
    uint32_t power;
    char atmp;
    char prefix_flag = 0;
    for(ipow=0; ipow<(sizeof(power10)/sizeof(power10[0])); ipow++)
    {
        atmp = '0';
        power = power10[ipow];
        while(val >= power)
        {
            atmp++;
            val -= power;
        }
        if(atmp > '0')
            prefix_flag = -1;
        if(prefix_flag)
            *dest++ = atmp;
    }
    *dest++ = val+'0';
    *dest = 0;
    return dest;
}

char* itoa(int32_t val, char* dest)
{
    if(val<0)
    {
        *dest++ = '-';
        return uitoa(-val, dest);
    }
    else
        return uitoa(val, dest);
}

static char _encHex(uint8_t v)
{
    if(v<10)
        return '0'+v;
    else
        return 'A'+v-10;
}

char* uitox(uint32_t val, char* dest, int digits)
{
    if(digits >= 8) *dest++ = _encHex((val>>28) & 0x0F);
    if(digits >= 7) *dest++ = _encHex((val>>24) & 0x0F);
    if(digits >= 6) *dest++ = _encHex((val>>20) & 0x0F);
    if(digits >= 5) *dest++ = _encHex((val>>16) & 0x0F);
    if(digits >= 4) *dest++ = _encHex((val>>12) & 0x0F);
    if(digits >= 3) *dest++ = _encHex((val>>8) & 0x0F);
    if(digits >= 2) *dest++ = _encHex((val>>4) & 0x0F);
    *dest++ = _encHex((val>>0) & 0x0F);
    *dest = 0;
    return dest;
}


#define CRC_POLYNOME 0xA001 //corresponds to X^16+X^15+X^2+X^0
crc16_t crc16(crc16_t crc, unsigned char data)
{
    char i;
    crc16_t res;
    //char common_flags;

    res = crc ^ (uint16_t)data;

    for(i=0; i<8; i++)
    {
        if(res % 2)
            res = (res>>1) ^ CRC_POLYNOME;
        else
            res >>= 1;
    }
    return res;
}

