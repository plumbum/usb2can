GDB   = $(TRGT)gdb
OPENOCD=openocd

OPENOCD_IFACE+=-f interface/stlink-v2.cfg -f target/stm32f1x.cfg

OPENOCD_LOADFILE+=$(BUILDDIR)/$(PROJECT).elf
# debug level
OPENOCD_CMD=-d0
# interface and board/target settings (using the OOCD target-library here)
OPENOCD_CMD+=$(OPENOCD_IFACE)
# initialize
OPENOCD_CMD+=-c init
# show the targets
OPENOCD_CMD+=-c targets
# commands to prepare flash-write
OPENOCD_CMD+= -c "reset halt"
# Unlock device
OPENOCD_CMD+= -c "stm32f1x unlock 0"
OPENOCD_CMD+= -c "reset halt"
# flash erase
OPENOCD_CMD+=-c "stm32f1x mass_erase 0"
# flash-write
OPENOCD_CMD+=-c "flash write_image $(OPENOCD_LOADFILE)"
# Verify
OPENOCD_CMD+=-c "verify_image $(OPENOCD_LOADFILE)"

#OPENOCD_CMD+=-c "stm32f1x lock 0"
# reset target
OPENOCD_CMD+=-c "reset run"
# terminate OOCD after programming
OPENOCD_CMD+=-c shutdown

# Upload and lock firmware
deploy: upload lock

upload: all
	@echo "Upload firmware with OPENOCD"
	$(OPENOCD) $(OPENOCD_CMD)

load: upload

openocd:
	@echo "Start openocd daemon"
	$(OPENOCD) $(OPENOCD_IFACE)

debug: all
	@echo "Debug with openocd daemon"
	$(GDB) -x ${COMMON}/fw_debug.gdb $<

lock:
	$(OPENOCD) -d0 $(OPENOCD_IFACE) -c init -c "reset halt" -c "stm32f1x lock 0" -c "reset halt" -c shutdown
	@echo "!!! После установки бита защиты необходимо полностью обесточить микроконтроллер."
	@echo "!!! При попытке работы через SWD, контроллер останавливается. И его придётся обесточивать."

unlock:
	$(OPENOCD) -d0 $(OPENOCD_IFACE) -c init -c "reset halt" -c "stm32f1x unlock 0" -c "reset halt" -c shutdown

reset:
	$(OPENOCD) -d0 $(OPENOCD_IFACE) -c init -c "reset run" -c shutdown

tags: 
	ctags -R --c++-kinds=+p --fields=+iaS --extra=+q . ${CHIBIOS}/os ${BOARDINC}

.PHONY: tags openocd debug upload stlink stload

stlink:
	st-util -m -p 3333

stload: all
	@echo Usage: 'load', 'break main', 'run', 'continue'
	$(GDB) -x ${COMMON}/fw_stload.gdb $<
