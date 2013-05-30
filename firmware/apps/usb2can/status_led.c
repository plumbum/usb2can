#include "status_led.h"
#include <hal.h>

static VirtualTimer vt1, vt2;

static ledoff_1(void* p)
{
    (void)p;
    palClearPad(IOPORT2, GPIOB_LED1);
}

static ledoff_2(void* p)
{
    (void)p;
    palClearPad(IOPORT2, GPIOB_LED2);
}

void statusLedPulse(int led, systime_t duration)
{
    chSysLock();
    switch(led)
    {
        case 1:
            if (chVTIsArmedI(&vt1))
                chVTResetI(&vt1);
            else
                palSetPad(IOPORT2, GPIOB_LED1);
            chVTSetI(&vt1, duration, ledoff_1, NULL);
            break;
        case 2:
            if (chVTIsArmedI(&vt2))
                chVTResetI(&vt2);
            else
                palSetPad(IOPORT2, GPIOB_LED2);
            chVTSetI(&vt2, duration, ledoff_2, NULL);
            break;
    }
    chSysUnlock();
}

