
openocd:
	openocd -f interface/stlink-v1.cfg -f target/stm32f1x_stlink.cfg

stlink:
	st-util -p 3333

load: $(PROJECT).elf
	$(GDB) -x ${COMMON}/fw_load.gdb $<

stload: $(PROJECT).elf
	@echo Use load, run, continue
	$(GDB) -x ${COMMON}/fw_stload.gdb $<

tags: 
	ctags -R --c++-kinds=+p --fields=+iaS --extra=+q . ${CHIBIOS}/os ${BOARDINC}

.PHONY: tags openocd stlink load

