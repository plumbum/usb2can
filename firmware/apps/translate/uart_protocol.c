#include "uart_protocol.h"
#include <hal.h>

// TODO use void canSTM32SetFilters(uint32_t can2sb, uint32_t num, const CANFilter *cfp);

const SerialConfig sc = {
    115200,
    0, 0, 0
};


/*
 * Internal loopback mode, 500KBaud, automatic wakeup, automatic recover
 * from abort mode.
 * See section 22.7.7 on the STM32 reference manual.
 */
static const CANConfig cancfg = {
    CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
    CAN_BTR_LBKM |
    CAN_BTR_SJW(0) | CAN_BTR_TS2(1) |
    CAN_BTR_TS1(8) | CAN_BTR_BRP(6)
};



#define sdGetMacro() sdGetTimeout(&SD_UP, PROTO_TIMEOUT)

static WORKING_AREA(uart_proto_wa, 1024);
static msg_t uart_proto_ps(void *p)
{
    (void)p;
    CANRxFrame rxmsg;
    CANTxFrame txmsg;
    msg_t c;
    int i;

    chRegSetThreadName("uart_proto");
    while(!chThdShouldTerminate())
    {
uart_proto_pkg_reset:
        /* Check incoming CAN message and translate it to Serial */
        /* Process all incoming messages */
        while (canReceive(&CAND_UP, CAN_ANY_MAILBOX, &rxmsg, TIME_IMMEDIATE) == RDY_OK)
        {
            if(rxmsg.IDE == CAN_IDE_STD) { 
                // Is standart block
                sdPut(&SD_UP, 0xAB);
                /// Put  flags & lenght
                sdPut(&SD_UP, (rxmsg.DLC & 0x0F) | ((rxmsg.RTR == CAN_RTR_REMOTE)?(1<<4):0));
                // Put command ID
                sdPut(&SD_UP, rxmsg.SID & 0xFF);
                sdPut(&SD_UP, (rxmsg.SID>>8) & 0xFF);
            } else {
                // Is extended block
                sdPut(&SD_UP, 0xAE);
                /// Put  flags & lenght
                sdPut(&SD_UP, (rxmsg.DLC & 0x0F) | ((rxmsg.RTR == CAN_RTR_REMOTE)?(1<<4):0));
                // Put command ID
                sdPut(&SD_UP, rxmsg.EID & 0xFF);
                sdPut(&SD_UP, (rxmsg.EID>>8) & 0xFF);
                sdPut(&SD_UP, (rxmsg.EID>>16) & 0xFF);
                sdPut(&SD_UP, (rxmsg.EID>>24) & 0xFF);
            }
            for(i=0; i<rxmsg.DLC; i++) {
                sdPut(&SD_UP, rxmsg.data8[i]);
            }
        }

        // Check serial commands
        c = sdGetTimeout(&SD_UP, TIME_IMMEDIATE);
        if(c<0) continue; // Skip
        switch(c) {
            case 0xCB: case 0xCE:
                switch(c) {
                    case 0xCB: // Standart frame
                        // Make frame on fly
                        txmsg.IDE = CAN_IDE_STD;
                        c = sdGetMacro(); if(c<0) continue;
                        txmsg.DLC = c & 0x0F; // Extract data len from flags
                        // check RTR flag
                        if(c & 0x10) txmsg.RTR = CAN_RTR_REMOTE;
                                else txmsg.RTR = CAN_RTR_DATA;
                        // Set standart command ID
                        c = sdGetMacro(); if(c<0) continue;
                        txmsg.SID = c;
                        c = sdGetMacro(); if(c<0) continue;
                        txmsg.SID |= (c<<8);
                        break;
                    case 0xCE: // Extended frame
                        // Make frame on fly
                        txmsg.IDE = CAN_IDE_EXT;
                        c = sdGetMacro(); if(c<0) continue;
                        txmsg.DLC = c & 0x0F; // Extract data len from flags
                        // check RTR flag
                        if(c & 0x10) txmsg.RTR = CAN_RTR_REMOTE;
                                else txmsg.RTR = CAN_RTR_DATA;
                        // Set standart command ID
                        c = sdGetMacro(); if(c<0) continue;
                        txmsg.EID = c;
                        c = sdGetMacro(); if(c<0) continue;
                        txmsg.EID |= (c<<8);
                        c = sdGetMacro(); if(c<0) continue;
                        txmsg.EID |= (c<<16);
                        c = sdGetMacro(); if(c<0) continue;
                        txmsg.EID |= (c<<24);
                        break;
                }

                // Get frame data
                for(i=0; i<txmsg.DLC; i++)
                {
                    c = sdGetMacro(); if(c<0) goto uart_proto_pkg_reset;
                    txmsg.data8[i] = c;
                }

                msg_t rcan = canTransmit(&CAND_UP, CAN_ANY_MAILBOX, &txmsg, MS2ST(100));
                if(rcan == RDY_OK) {
                    sdPut(&SD_UP, 0xCB);
                    sdPut(&SD_UP, 0xCA);
                } else {
                    sdPut(&SD_UP, 0xCB);
                    sdPut(&SD_UP, 0xCF);
                    sdPut(&SD_UP, 0x00);
                }
                    
                break;
        }
    }
    return 0;
}

void uartpInit(void)
{
    /* Activates the Serial driver. */
    sdStart(&SD_UP, &sc);
    /* Activates the CAN driver 1. */
    canStart(&CAND_UP, &cancfg);

    chThdCreateStatic(uart_proto_wa, sizeof(uart_proto_wa), NORMALPRIO - 7, uart_proto_ps, NULL);
}

/*

My protocol suggestion 
--------------------------------------------------
Send CAN frames

Send standard frame
0xcb | flags | id | id | data 1-8 bytes
0xcb - start command
flags bits:
- frame len (bits 0:3)
- rtr (bit 4)

Answer
0xcb 0xca - OK
0xcb 0xcf error_status - NOK

Send extended frame
0xce | flags | id | id | id | id | data 1-8 bytes
flags bits:
[0:3] - data lenght
[4] - rtr

Answer
0xce 0xca - OK
0xce 0xcf error_status - NOK

--------------------------------------------------
Receive CAN frames
0xab | flags | id | id | data 1-8 bytes
0xae | flags | id | id | id | id | data 1-8 bytes

--------------------------------------------------
Control menu

0xcc - start command

0xcc 0x01 - INFO submenu
0xcc 0x01 0x01 - firmware version
0xcc 0x01 0x02 - can speed info
0xcc 0x01 0x03 - rs speed info

0xcc 0x02 - SPEED submenu
0xcc 0x02 nr - set can speed numer
0xcc 0x02 btr1 btr2 - set can speed with btr

0xcc 0x03 - DISPLAY submenu
0xcc 0x03 nr - set display can frame to format nr
eg.
ID RTR LEN DATA
or
ID=xxx RTR=x LEN=x DATA=xx xx xx

0xcc 0x04 - FILTER submenu
0xcc 0x04 0x01 - display filter
0xcc 0x04 0x02 - clear filter
0xcc 0x04 0x03 xx xx - add standard id to filter
0xcc 0x04 0x04 xx xx xx xx - add standard id range to filter
0xcc 0x04 0x05 xx xx xx xx(...) - add extended id to filter
0xcc 0x04 0x06 xx xx xx xx(...) - add extended id range to filter


0xcc 0x05 - START/STOP submenu
0xcc 0x05 0x01 start/stop sniffing


Answer
0xcc 0xca - OK
0xcc 0xcf error_status - NOK

 */

