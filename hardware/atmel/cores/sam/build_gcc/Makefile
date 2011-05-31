# Makefile for compiling libboard
BOARD = 

SUBMAKE_OPTIONS=--no-builtin-rules --no-builtin-variables

#-------------------------------------------------------------------------------
# Rules
#-------------------------------------------------------------------------------

all: sam3s_ek

.PHONY: sam3s_ek
sam3s_ek:
	@echo --- Making sam3s_ek
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f arduino_sam3s_ek.mk
	@$(MAKE) $(SUBMAKE_OPTIONS) -f arduino_sam3s_ek.mk

.PHONY: clean
clean:
	@echo --- Cleaning sam3s_ek
	@$(MAKE) $(SUBMAKE_OPTIONS) -f arduino_sam3s_ek.mk $@
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f arduino_sam3s_ek.mk $@


