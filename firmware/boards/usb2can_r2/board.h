/*
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio

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

#ifndef _BOARD_H_
#define _BOARD_H_

/*
 * Setup for the STM32F103C8T6 Module often described as "Arduino"-like
 * on eBay, typically marked "www.vcc-gnd.com".
 */

/*
 * Board identifier.
 */
#define BOARD_USB2CAN_R2
#define BOARD_NAME              "USB2CAN (via serial)"

/*
 * Board frequencies.
 */
#define STM32_LSECLK            32768
#define STM32_HSECLK            8000000

/*
 * MCU type, supported types are defined in ./os/hal/platforms/hal_lld.h.
 *
 * Only xB (128KB Flash) is defined, but it's identical to the
 * x8 version (64KB Flash) except for the Flash region size in the
 * linker script. For x8 parts use xB here and change to the x8 linker
 * script in the project Makefile.
 */
#define STM32F103xB

/*
 * IO pins assignments
 *
 * numbering is sorted by onboard/connectors, as from the schematics in
 * http://www.vcc-gnd.com/read.php?tid=369
 */

/* on-board */

#define GPIOB_LED1      1
#define GPIOB_LED2      2
#define SetLed1()       palSetPad(GPIOB, GPIOB_LED1);
#define ClrLed1()       palClearPad(GPIOB, GPIOB_LED1);
#define ToggleLed1()    palTogglePad(GPIOB, GPIOB_LED1);
#define SetLed2()       palSetPad(GPIOB, GPIOB_LED2);
#define ClrLed2()       palClearPad(GPIOB, GPIOB_LED2);
#define ToggleLed2()    palTogglePad(GPIOB, GPIOB_LED2);

#define GPIOC_BUTTON 13
#define GetButton() (GPIOC & (1<<GPIOC_BUTTON))

// #define GPIOA_USBDM             11      // pin 8
// #define GPIOA_USBDP             12      // pin 9

// #define GPIOC_OSC32_IN          14
// #define GPIOC_OSC32_OUT         15

/*
 * I/O ports initial setup, this configuration is established soon after reset
 * in the initialization code.
 *
 * The digits have the following meaning:
 *   0 - Analog input.
 *   1 - Push Pull output 10MHz.
 *   2 - Push Pull output 2MHz.
 *   3 - Push Pull output 50MHz.
 *   4 - Digital input.
 *   5 - Open Drain output 10MHz.
 *   6 - Open Drain output 2MHz.
 *   7 - Open Drain output 50MHz.
 *   8 - Digital input with PullUp or PullDown resistor depending on ODR.
 *   9 - Alternate Push Pull output 10MHz.
 *   A - Alternate Push Pull output 2MHz.
 *   B - Alternate Push Pull output 50MHz.
 *   C - Reserved.
 *   D - Alternate Open Drain output 10MHz.
 *   E - Alternate Open Drain output 2MHz.
 *   F - Alternate Open Drain output 50MHz.
 * Please refer to the STM32 Reference Manual for details.
 */

/*
 * Port A setup.
 * Everything input with pull-up except:
 * PA9  - TXD1
 * PA10 - RXD1
 * PA11 - CTS1
 * PA12 - RTS1
 * PA13 - SWDIO
 * PA14 - SWCLK
 */
#define VAL_GPIOACRL            0x88888888      /*  PA7...PA0 */
#define VAL_GPIOACRH            0x888388B8      /* PA15...PA8 */
#define VAL_GPIOAODR            0xFFFFFFFF

/*
 * Port B setup.
 * Everything input with pull-up except:
 * PB1  - LED1
 * PB2  - LED2
 * PB4  - #SUSPEND
 * PB5  - #CPRST
 * PB6  - SCL1
 * PB7  - SDA1
 * PB8  - CAN_RX
 * PB9  - CAN_TX
 * PB10 - LCD_SI
 * PB11 - #LCD_CS
 * PB12 - LCD_CK
 */
#define VAL_GPIOBCRL            0xFF888338      /*  PB7...PB0 */
#define VAL_GPIOBCRH            0x888B3BB8      /* PB15...PB8 */
#define VAL_GPIOBODR            (0xFFFFFFFF & ~(1<<GPIOB_LED1) & ~(1<<GPIOB_LED2))

/*
 * Port C setup.
 * Everything input with pull-up except:
 * PC13 - Digital input (BUTTON).
 */
#define VAL_GPIOCCRL            0x88888888      /*  PC7...PC0 */
#define VAL_GPIOCCRH            0x44488888      /* PC15...PC8 */
#define VAL_GPIOCODR            (0xFFFFFFFF & ~(1<<GPIOC_BUTTON))

/*
 * Port D setup.
 * Everything input with pull-up except:
 * PD0  - Normal input (XTAL).
 * PD1  - Normal input (XTAL).
 */
#define VAL_GPIODCRL            0x88888844      /*  PD7...PD0 */
#define VAL_GPIODCRH            0x88888888      /* PD15...PD8 */
#define VAL_GPIODODR            0xFFFFFFFF

/*
 * Port E setup.
 * Everything input with pull-up except:
 */
#define VAL_GPIOECRL            0x88888888      /*  PE7...PE0 */
#define VAL_GPIOECRH            0x88888888      /* PE15...PE8 */
#define VAL_GPIOEODR            0xFFFFFFFF

/*
 * USB bus activation macro, required by the USB driver.
 */
#define usb_lld_connect_bus(usbp)	/* always connected */

/*
 * USB bus de-activation macro, required by the USB driver.
 */
#define usb_lld_disconnect_bus(usbp)	/* always connected */

#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* _BOARD_H_ */
