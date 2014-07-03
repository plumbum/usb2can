#include <ch.h>
#include <hal.h>

#include "uart_protocol.h"

/*
 * Application entry point.
 */
int main(void) {

    /*
     * System initializations.
     * - HAL initialization, this also initializes the configured device drivers
     *   and performs the board-specific initializations.
     * - Kernel initialization, the main() function becomes a thread and the
     *   RTOS is active.
     */
    halInit();
    chSysInit();

    uartpInit();

    /* Normal main() thread activity, in this demo it does nothing. */
    while (TRUE)
    {
        chThdSleepMilliseconds(500);
    }
    return 0;
}
