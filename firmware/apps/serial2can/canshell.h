#ifndef _CANSHELL_H_
#define _CANSHELL_H_

#include "ch.h"
#include "hal.h"

#define CAN_SHELL_MAX_LINE_LENGTH 128

void canshellInit(BaseSequentialStream *chp);

#endif /* _CANSHELL_H_ */

