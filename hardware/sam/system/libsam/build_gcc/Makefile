# Makefile for compiling libchip

SUBMAKE_OPTIONS=--no-builtin-rules --no-builtin-variables

#-------------------------------------------------------------------------------
# Rules
#-------------------------------------------------------------------------------

# libchip_sam3s4c_gcc_rel.a
all: libchip_sam3s4c_gcc_dbg.a 

libchip_sam3s4c_gcc_dbg.a:
	@echo --- Making $@
	@$(MAKE) CHIP=__SAM3S4C__ DEBUG=1 $(SUBMAKE_OPTIONS) -f sam3.mk

libchip_sam3s4c_gcc_rel.a:
	@echo --- Making $@
	@$(MAKE) CHIP=__SAM3S4C__ $(SUBMAKE_OPTIONS) -f sam3.mk

.PHONY: clean
clean:
	@echo --- Cleaning sam3s4c release and debug
	@$(MAKE) CHIP=__SAM3S4C__ $(SUBMAKE_OPTIONS) -f sam3.mk $@
	@$(MAKE) CHIP=__SAM3S4C__ DEBUG=1 $(SUBMAKE_OPTIONS) -f sam3.mk $@


