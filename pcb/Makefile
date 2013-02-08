PROJECT=ser2can

GERB = $(PROJECT).TXT
GERB += $(PROJECT).GTO
GERB += $(PROJECT).GTS
GERB += $(PROJECT).GTL
GERB += $(PROJECT).GBL
GERB += $(PROJECT).GBS
GERB += $(PROJECT).GBO

all: gerbv

gerbv:
	gerbv $(GERB)

zip:
	zip $(PROJECT)_gerber.zip $(GERB)

clean:
	rm -f $(GERB)

.PHONY: gerbv zip clean

