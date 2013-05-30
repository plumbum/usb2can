#include "shell_cmd.h"

#include <hal.h>

#include <shell.h>
#include <chprintf.h>

#define SHELL_WA_SIZE   THD_WA_SIZE(2048)

static void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[]);
static void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[]);

static Thread *shelltp = NULL;

static const ShellCommand commands[] = {
    {"mem", cmd_mem},
    {"threads", cmd_threads},
    {NULL, NULL}
};

static const ShellConfig shell_cfg1 = {
    (BaseSequentialStream *)&SD_SHELL,
    commands
};


static void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[])
{
    size_t n, size;

    (void)argv;
    if (argc > 0) {
        chprintf(chp, "Usage: mem\r\n");
        return;
    }
    n = chHeapStatus(NULL, &size);
    chprintf(chp, "core free memory : %u bytes\r\n", chCoreStatus());
    chprintf(chp, "heap fragments   : %u\r\n", n);
    chprintf(chp, "heap free total  : %u bytes\r\n", size);
}

static void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[])
{
    static const char *states[] = {THD_STATE_NAMES};
    Thread *tp;

    (void)argv;
    if (argc > 0) {
        chprintf(chp, "Usage: threads\r\n");
        return;
    }
    chprintf(chp, "    addr    stack prio refs     state time\r\n");
    tp = chRegFirstThread();
    do {
        chprintf(chp, "%.8lx %.8lx %4lu %4lu %9s %lu\r\n",
                 (uint32_t)tp, (uint32_t)tp->p_ctx.r13,
                 (uint32_t)tp->p_prio, (uint32_t)(tp->p_refs - 1),
                 states[tp->p_state], (uint32_t)tp->p_time);
        tp = chRegNextThread(tp);
    } while (tp != NULL);
}

void shellcmdInit(void)
{
    sdStart(&SD_SHELL, NULL);
    chprintf((BaseSequentialStream*)&SD_SHELL, "USB2CAN converter\r\n");
    chprintf((BaseSequentialStream*)&SD_SHELL, "Build: " __DATE__ " " __TIME__ "\r\n");
    chprintf((BaseSequentialStream*)&SD_SHELL, "Flash size: %dkb\r\n", *DES_FSIZE);
    chprintf((BaseSequentialStream*)&SD_SHELL, "Unique device ID: %.8x %.8x %.8x\r\n", U_ID[0], U_ID[1], U_ID[2]);

    shelltp = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);

}

