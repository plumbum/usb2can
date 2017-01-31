#include "canshell.h"

#include <string.h>

#include "chprintf.h"

#define GET_LINE_ERROR (-1)
#define GET_LINE_EOT (-4)

#define ASK_OK(chp) chSequentialStreamPut((chp), 13)
#define ASK_ERR(chp) chSequentialStreamPut((chp), 7)

#define MODE_RESET 0
#define MODE_OPERATE 1
#define MODE_LISTEN 2

int canshellGetLine(BaseSequentialStream *chp, char *line, unsigned size, bool echo);

static THD_WORKING_AREA(canshell_wa, 512);
static THD_FUNCTION(canshell_ps, p)
{

    char line[CAN_SHELL_MAX_LINE_LENGTH];

    BaseSequentialStream *chp = (BaseSequentialStream *)p;

    chRegSetThreadName("canshell");

    bool echo = false;
    int mode = MODE_RESET;

    while (!chThdShouldTerminateX())
    {
        int llen = canshellGetLine(chp, line, sizeof(line), echo);
        if (llen <= 0)
        {
            continue;
        }
        // Mode specified commands
        switch (mode)
        {
        case MODE_OPERATE:
            switch (line[0])
            {
            case 't': // Transmit std 11bit frame
                break;
            case 'T': // Transmit ext 29bit frame
                break;
            case 'r': // Transmit remote std frame
                break;
            case 'R': // Transmit remote ext frame
                break;
            }
        // No need break here
        case MODE_LISTEN:
            switch (line[0])
            {
            case 'C':
                // @TODO disable CAN
                mode = MODE_RESET;
                break;
            case 'A':
                chprintf(chp, "A%02x\r", 0);
                break;
            case 'E':
                chprintf(chp, "E%02x\r", 0);
                break;
            case 'F':
                chprintf(chp, "F%02x\r", 0);
                break;
            }
            break;
        case MODE_RESET:
            switch (line[0])
            {
            case 'L': // listen mode
                // @TODO init CAN
                mode = MODE_LISTEN;
                break;
            case 'O': // Operate mode
                // @TODO init CAN
                mode = MODE_OPERATE;
                break;
            case 'M':
                break;
            case 'm':
                break;
            case 'S':
                break;
            case 's':
                break;
            }
            break;
        }
        // Common commands
        switch (line[0]) // Command
        {
        case 'N': // @TODO Serial number
            chprintf(chp, "N%s\r", "alphanum");
            break;
        case 'V': // @TODO Version
            chprintf(chp, "V%02x%02x\r", 1, 2);
            break;
        case 'v': // @TODO Detailed FW version
            chprintf(chp, "v%02x%02x\r", 3, 4);
            break;
        case 'G': // @TODO Gxx[CR] Read register content from SJA1000 controller.
            chprintf(chp, "G%02x\r", 0);
            break;
        case 'W': // @TODO Wrrdd[CR] Write SJA1000 register with data
            ASK_ERR(chp);
            break;
        case 'Z': // @TODO Toggle timestamp
            ASK_ERR(chp);
            break;
        case 'Q': // Echo on (humanize protocol)
            echo = true;
            ASK_OK(chp);
            break;
        case 'q': // Echo off (original protocol)
            echo = false;
            ASK_OK(chp);
            break;
        default:
            ASK_ERR(chp);
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
 * @return              Line lenght
 *
 */
int canshellGetLine(BaseSequentialStream *chp, char *line, unsigned size, bool echo)
{
    char *p = line;

    while (true)
    {
        char c;

        if (chSequentialStreamRead(chp, (uint8_t *)&c, 1) == 0)
            return -2;

        if (echo)
        {
            if (c == 4)
            {
                chprintf(chp, "^D");
                return -1;
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
        }
        if (c == '\r')
        {
            if (echo)
            {
                chprintf(chp, "\r\n");
            }
            *p = 0;
            return p - line;
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
