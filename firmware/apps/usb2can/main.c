#include <ch.h>
#include <hal.h>

#include <chprintf.h>
#include "status_led.h"
#include "shell_cmd.h"


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

    shellcmdInit();

    palClearPad(IOPORT2, GPIOB_LED1);

    while (TRUE)
    {
        statusLedPulse(1, MS2ST(50));
        statusLedPulse(2, MS2ST(250));
        chThdSleepMilliseconds(1000);
    }
    return 0;
}
