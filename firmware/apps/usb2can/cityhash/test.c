#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "city.h"

int main(int argc, char** argv) {

    char s[100];

    int i;
    for(i=1000; i<10000; i++) {
        sprintf(s, "Test %d", i);
        uint64_t hash = CityHash64(s, strlen(s));

        printf("0x%08x%08x, %s\r\n",
            (uint32_t)((hash>>32)&0xFFFFFFFF),
            (uint32_t)(hash&0xFFFFFFFF), s);
    }

    return 0;
}

