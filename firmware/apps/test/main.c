/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"

#include "chprintf.h"


/*
 * This is a periodic thread that does absolutely nothing except increasing
 * a seconds counter.
 */
static WORKING_AREA(waThread1, 128);
static msg_t Thread1(void *arg) {

    (void)arg;
    chRegSetThreadName("counter");
    while (TRUE) {
        palTogglePad(IOPORT2, GPIOB_LED1);
        palTogglePad(IOPORT2, GPIOB_LED2);
        chThdSleepMilliseconds(500);
    }
    return 0;
}

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

    RCC->CFGR |= RCC_CFGR_MCO_SYSCLK;

    /*
     * Activates the serial driver 1 using the driver default configuration.
     */
    sdStart(&SD1, NULL);

    chprintf((BaseSequentialStream*)&SD1, "Hello world!\r\n");

    /*
     * If the user button is pressed after the reset then the test suite is
     * executed immediately before activating the various device drivers in
     * order to not alter the benchmark scores.
     */
    // if (palReadPad(GPIOA, GPIOA_BUTTON))
    //    TestThread(&SD1);

    /*
     * Initializes the SPI driver 1.
     */
    // spiStart(&SPID1, &spicfg);

    /*
     * Initializes the ADC driver 1.
     * The pin PC0 on the port GPIOC is programmed as analog input.
     */
    // adcStart(&ADCD1, NULL);
    // palSetGroupMode(GPIOC, PAL_PORT_BIT(0), 0, PAL_MODE_INPUT_ANALOG);

    /*
     * Initializes the PWM driver 3, re-routes the TIM3 outputs, programs the
     * pins as alternate functions.
     * Note, the AFIO access routes the TIM3 output pins on the PC6...PC9
     * where the LEDs are connected.
     */
    /*
    pwmStart(&PWMD3, &pwmcfg);
    AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP_0 | AFIO_MAPR_TIM3_REMAP_1;
    palSetGroupMode(GPIOC, PAL_PORT_BIT(GPIOC_LED3) | PAL_PORT_BIT(GPIOC_LED4),
                    0,
                    PAL_MODE_STM32_ALTERNATE_PUSHPULL);
                    */

    /*
     * Creates the example thread.
     */
    chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

    /*
     * Normal main() thread activity, in this demo it does nothing except
     * sleeping in a loop and check the button state, when the button is
     * pressed the test procedure is launched with output on the serial
     * driver 1.
     */
    uint32_t cnt = 0;
    while (TRUE)
    {
        /*
        if (palReadPad(GPIOA, GPIOA_BUTTON))
            TestThread(&SD1);
            */
        chprintf((BaseSequentialStream*)&SD1, "Count %10d\r\n", cnt++);
        chThdSleepMilliseconds(1000);
    }
}
