#include <ch.h>
#include <hal.h>

#include <chprintf.h>
#include "status_led.h"

#define DES_FSIZE ((uint16_t*)0x1FFFF7E0)
#define U_ID      ((uint32_t*)0x1FFFF7E8)


int main(void)
{

    /*
     * System initializations.
     * - HAL initialization, this also initializes the configured device drivers
     *   and performs the board-specific initializations.
     * - Kernel initialization, the main() function becomes a thread and the
     *   RTOS is active.
     */
    halInit();
    chSysInit();

    sdStart(&SD1, NULL);
    chprintf((BaseSequentialStream*)&SD1, "USB2CAN " __DATE__ " " __TIME__ "\r\n");
    chprintf((BaseSequentialStream*)&SD1, "Flash size %dkb\r\n", *DES_FSIZE);
    chprintf((BaseSequentialStream*)&SD1, "Unique device ID %.8x %.8x %.8x\r\n", U_ID[0], U_ID[1], U_ID[2]);

    palClearPad(IOPORT2, GPIOB_LED1);

    while (TRUE)
    {
        statusLedPulse(1, MS2ST(50));
        statusLedPulse(2, MS2ST(250));
        chThdSleepMilliseconds(1000);
    }
    return 0;
}
