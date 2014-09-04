#include "uart_protocol.h"
#include <hal.h>


const SerialConfig sc = {
    115200,
    0, 0, 0
};


#define GET_CHAR(var) do { msg_t __c = sdGetTimeout(&SD_PS, PROTO_TIMEOUT); if(__c<0) continue; var = __c; } while(0) 

static WORKING_AREA(uart_proto_wa, 256);
static msg_t uart_proto_ps(void *p)
{
    (void)p;
    CANTxFrame txmsg;
    msg_t tmp;

    chRegSetThreadName("uart_proto");
    while(!chThdShouldTerminate())
    {
        GET_CHAR(tmp);
        switch(tmp) {
            case 0xCB: // Standart frame
                {
                    // Make frame on fly
                    txmsg.IDE = CAN_IDE_STD;
                    GET_CHAR(tmp); // Get flags
                    txmsg.DLC = tmp & 0x0F; // Extract data len from flags
                    txmsg.SID
                    uint8_t len = flags & 0x0F;
                    uint16_t sid;
                    
                }
                break;
            case 0xCE: // Extended frame
                break;
    }
    return 0;
}

void uartpInit(void)
{
    sdStart(&SD_PS, &sc);
    chThdCreateStatic(uart_proto_wa, sizeof(uart_proto_wa), NORMALPRIO + 7, uart_proto_ps, NULL);
}

/*

My protocol suggestion 
--------------------------------------------------
Send CAN frames

Send standard frame
0xcb | flags | id | id | data 1-8 bytes
0xcb - start command
flags bits:
- frame len (bits 0-3)
- rtr (bit 4)

Answer
0xcb 0xca - OK
0xcb 0xcf error_status - NOK

Send extended frame
0xce | flags | id | id | id | id | data 1-8 bytes
flags bits:
[0:4] - data lenght
[5] - rtr

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

