#ifndef _UTILS_H_
#define _UTILS_H_

#include <inttypes.h>

typedef uint16_t crc16_t;

#define POWER10_SIZE 9
extern const uint32_t power10[POWER10_SIZE];

int numericLength(int32_t v);

char* uitoa(uint32_t val, char* dest);
char* itoa(int32_t val, char* dest);

char* uitox(uint32_t val, char* dest, int digits);

crc16_t crc16(crc16_t crc, unsigned char data);

#endif /* _UTILS_H_ */

