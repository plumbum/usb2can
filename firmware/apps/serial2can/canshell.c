#include "canshell.h"

#include <string.h>

#include "ch.h"
#include "hal.h"
#include "chprintf.h"

bool canshellGetLine(BaseSequentialStream *chp, char *line, unsigned size, bool echo);

static THD_WORKING_AREA(canshell_wa, 512);
static THD_FUNCTION(canshell_ps, p)
{

    char line[CAN_SHELL_MAX_LINE_LENGTH];

    BaseSequentialStream *chp = (BaseSequentialStream *)p;

    chRegSetThreadName("canshell");

    while (!chThdShouldTerminateX())
    {

        if (shellGetLine(chp, line, sizeof(line), true))
        {
            chprintf(chp, "\r\nlogout");
            break;
        }
    }
}

void canshellInit(BaseSequentialStream *chp)
{
    chThdCreateStatic(canshell_wa, sizeof(canshell_wa), NORMALPRIO + 7, canshell_ps, (void *)chp);
}

/**
 * @brief   Reads a whole line from the input channel.
 *
 * @param[in] chp       pointer to a @p BaseSequentialStream object
 * @param[in] line      pointer to the line buffer
 * @param[in] size      buffer maximum length
 * @param[in] echo      enable local echo
 * @return              The operation status.
 * @retval true         the channel was reset or CTRL-D pressed.
 * @retval false        operation successful.
 *
 * @api
 */
bool canshellGetLine(BaseSequentialStream *chp, char *line, unsigned size, bool echo)
{
    char *p = line;

    while (true)
    {
        char c;

        if (chSequentialStreamRead(chp, (uint8_t *)&c, 1) == 0)
            return true;

        if (echo)
        {
            if (c == 4)
            {
                chprintf(chp, "^D");
                return true;
            }
            if ((c == 8) || (c == 127))
            {
                if (p != line)
                {
                    chSequentialStreamPut(chp, 0x08);
                    chSequentialStreamPut(chp, 0x20);
                    chSequentialStreamPut(chp, 0x08);
                    p--;
                }
                continue;
            }
            if (c == '\r')
            {
                chprintf(chp, "\r\n");
                *p = 0;
                return false;
            }
        }
        if (c < 0x20)
            continue;
        if (p < line + size - 1)
        {
            if (echo)
            {
                chSequentialStreamPut(chp, c);
            }
            *p++ = (char)c;
        }
    }
}
