# GDB startup script for debugging program in STM32F103 flash ROM
# $Id$

# Connect to the OpenOCD server
target remote localhost:3333 

monitor adapter_khz 10

monitor reset halt

monitor stm32f1x unlock 0
monitor stm32f1x mass_erase 0
monitor flash probe 0

#monitor reset run
monitor sleep 1000

monitor adapter_khz 1000

load

# Break at beginning of main()
#continue

#info reg

continue

