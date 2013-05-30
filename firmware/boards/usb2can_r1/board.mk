# Required include directories
BOARDINC = ${BOARDS}/usb2can_r1

# List of all the board related files.
BOARDSRC = ${BOARDINC}/board.c

# Define linker script file here
LDSCRIPT= $(BOARDINC)/STM32F103x8.ld

