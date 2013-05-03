GDB   = $(TRGT)gdb
OPENOCD=openocd

OPENOCD_IFACE+=-f interface/jlink.cfg -f target/stm32f1x.cfg

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
# flash erase
OPENOCD_CMD+=-c "stm32f1x mass_erase 0"
# flash-write
OPENOCD_CMD+=-c "flash write_image $(OPENOCD_LOADFILE)"
# Verify
OPENOCD_CMD+=-c "verify_image $(OPENOCD_LOADFILE)"
# reset target
OPENOCD_CMD+=-c "reset run"
# terminate OOCD after programming
OPENOCD_CMD+=-c shutdown

.PHONY: openocd tags

prog program: $(OPENOCD_LOADFILE)
	@echo "Programming with OPENOCD"
	$(OPENOCD) $(OPENOCD_CMD)

load: $(OPENOCD_LOADFILE)
	$(GDB) -x ../common/fw_load.gdb $<

debug: $(OPENOCD_LOADFILE)
	$(GDB) -x ../common/fw_debug.gdb $<

openocd:
	$(OPENOCD) $(OPENOCD_IFACE)

stlink:
	st-util -v2 -1 -p 3333

stload: $(PROJECT).elf 
	@echo Usage: 'load', 'break main', 'run', 'continue'
	$(GDB) -x ../common/fw_load_stlink.gdb $<


