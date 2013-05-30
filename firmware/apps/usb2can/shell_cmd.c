#include "shell_cmd.h"

#include <hal.h>

#include <chprintf.h>
#include "shell.h"

#define SHELL_WA_SIZE   THD_WA_SIZE(2048)

static Thread *shelltp = NULL;

static const ShellCommand commands[] = {
    {NULL, NULL}
};

static const ShellConfig shell_cfg1 = {
    (BaseSequentialStream *)&SD_SHELL,
    commands
};


void shellcmdInit(void)
{
    sdStart(&SD_SHELL, NULL);
    chprintf((BaseSequentialStream*)&SD_SHELL, "\r\n## USB2CAN converter\r\n");
    chprintf((BaseSequentialStream*)&SD_SHELL, "## Build: " __DATE__ " " __TIME__ "\r\n");
    chprintf((BaseSequentialStream*)&SD_SHELL, "## Flash: %dkb\r\n", *DES_FSIZE);
    chprintf((BaseSequentialStream*)&SD_SHELL, "## UID: %.8x %.8x %.8x\r\n", U_ID[0], U_ID[1], U_ID[2]);

    shelltp = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);

}

