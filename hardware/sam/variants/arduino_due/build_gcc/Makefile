# Makefile for compiling libboard
BOARD = 

SUBMAKE_OPTIONS=--no-builtin-rules --no-builtin-variables

#-------------------------------------------------------------------------------
# Rules
#-------------------------------------------------------------------------------

all: arduino_due

.PHONY: arduino_due
arduino_due:
	@echo --- Making arduino_due
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libvariant_arduino_due.mk
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libvariant_arduino_due.mk

.PHONY: clean
clean:
	@echo --- Cleaning arduino_due
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libvariant_arduino_due.mk $@
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libvariant_arduino_due.mk $@


