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

#include "ch.h"
#include "hal.h"

#include <string.h>

#include "chprintf.h"
/*
 * Internal loopback mode, 500KBaud, automatic wakeup, automatic recover
 * from abort mode.
 * See section 22.7.7 on the STM32 reference manual.
 */
static const CANConfig cancfg = {
    CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
    CAN_BTR_LBKM | CAN_BTR_SJW(0) | CAN_BTR_TS2(1) |
        CAN_BTR_TS1(8) | CAN_BTR_BRP(6)};

/*
 * Receiver thread.
 */
static THD_WORKING_AREA(can_rx_wa, 256);
static THD_FUNCTION(can_rx, p)
{
  event_listener_t el;
  CANRxFrame rxmsg;

  (void)p;
  chRegSetThreadName("receiver");
  chEvtRegister(&CAND1.rxfull_event, &el, 0);
  while (!chThdShouldTerminateX())
  {
    if (chEvtWaitAnyTimeout(ALL_EVENTS, MS2ST(100)) == 0)
      continue;
    while (canReceive(&CAND1, CAN_ANY_MAILBOX, &rxmsg, TIME_IMMEDIATE) == MSG_OK)
    {
      /* Process message.*/
      ToggleLed1();
    }
  }
  chEvtUnregister(&CAND1.rxfull_event, &el);
}

/*
 * Transmitter thread.
 */
static THD_WORKING_AREA(can_tx_wa, 256);
static THD_FUNCTION(can_tx, p)
{
  CANTxFrame txmsg;

  (void)p;
  chRegSetThreadName("transmitter");
  txmsg.IDE = CAN_IDE_EXT;
  txmsg.EID = 0x01234567;
  txmsg.RTR = CAN_RTR_DATA;
  txmsg.DLC = 8;
  txmsg.data32[0] = 0x55AA55AA;
  txmsg.data32[1] = 0x00FF00FF;

  while (!chThdShouldTerminateX())
  {
    canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, MS2ST(100));
    chThdSleepMilliseconds(500);
  }
}

/* Shell on serial */

/*
#define SHELL_WA_SIZE THD_WORKING_AREA_SIZE(2048)
#define TEST_WA_SIZE THD_WORKING_AREA_SIZE(256)

static void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[])
{
  size_t n, size;

  (void)argv;
  if (argc > 0)
  {
    chprintf(chp, "Usage: mem\r\n");
    return;
  }
  n = chHeapStatus(NULL, &size);
  chprintf(chp, "core free memory : %u bytes\r\n", chCoreGetStatusX());
  chprintf(chp, "heap fragments   : %u\r\n", n);
  chprintf(chp, "heap free total  : %u bytes\r\n", size);
}

static void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[])
{
  static const char *states[] = {CH_STATE_NAMES};
  thread_t *tp;

  (void)argv;
  if (argc > 0)
  {
    chprintf(chp, "Usage: threads\r\n");
    return;
  }
  chprintf(chp, "    addr    stack prio refs     state time\r\n");
  tp = chRegFirstThread();
  do
  {
    chprintf(chp, "%08lx %08lx %4lu %4lu %9s\r\n",
             (uint32_t)tp, (uint32_t)tp->p_ctx.r13,
             (uint32_t)tp->p_prio, (uint32_t)(tp->p_refs - 1),
             states[tp->p_state]);
    tp = chRegNextThread(tp);
  } while (tp != NULL);
}

static void cmd_test(BaseSequentialStream *chp, int argc, char *argv[])
{
  thread_t *tp;

  (void)argv;
  if (argc > 0)
  {
    chprintf(chp, "Usage: test\r\n");
    return;
  }
  chprintf(chp, "Hello test world!\r\n");
}

static const ShellCommand commands[] = {
    {"mem", cmd_mem},
    {"threads", cmd_threads},
    {"test", cmd_test},
    {NULL, NULL}};

static const ShellConfig shell_cfg1 = {
    (BaseSequentialStream *)&SD1,
    commands};

microrl_t rl;
microrl_t *prl = &rl;

void print(const char *str)
{
  sdWrite(&SD1, str, strlen(str));
}

int execute(int argc, const char *const *argv)
{
  int i;
  for (i = 0; i < argc; i++)
  {
    chprintf(&SD1, "%d: %s\r\n", i, argv[i]);
  }
}

void sigint(void)
{
  chprintf(&SD1, "^C catched!\r\n");
}
*/


/*
 * Application entry point.
 */
int main(void)
{

  // thread_t *shelltp = NULL;

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
  // shellInit();

  // shelltp = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);

  /*
   * Activates the CAN driver 1.
   */
  canStart(&CAND1, &cancfg);
  // canSTM32SetFilters();

  /*
   * Starting the transmitter and receiver threads.
   */
  chThdCreateStatic(can_rx_wa, sizeof(can_rx_wa), NORMALPRIO + 7, can_rx, NULL);
  chThdCreateStatic(can_tx_wa, sizeof(can_tx_wa), NORMALPRIO + 7, can_tx, NULL);

  /*
  // call init with ptr to microrl instance and print callback
  chprintf(&SD1, "Init shell\r\n");
	microrl_init (prl, print);
	// set callback for execute
  chprintf(&SD1, "Set execute callback\r\n");
	microrl_set_execute_callback (prl, execute);

#ifdef _USE_COMPLETE
	// set callback for completion
	// microrl_set_complete_callback (prl, complet);
#endif
	// set callback for Ctrl+C
  chprintf(&SD1, "Set SIGINT callback\r\n");
	microrl_set_sigint_callback (prl, sigint);
  chprintf(&SD1, "Wait commands\r\n");
  */

  while (1)
  {
    /*
    char c;
    size_t ok = sdRead(&SD1, &c, 1);
    // put received char from stdin to microrl lib
    if (ok > 0)
    {
      microrl_insert_char(prl, (int)c);
    }
    // chThdSleepMilliseconds(500);
    */
  }

  return 0;
}
