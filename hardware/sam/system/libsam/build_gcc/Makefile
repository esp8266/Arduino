# Makefile for compiling libchip

SUBMAKE_OPTIONS=--no-builtin-rules --no-builtin-variables

#-------------------------------------------------------------------------------
# Rules
#-------------------------------------------------------------------------------

# libchip_sam3s4_gcc_rel.a
all: libchip_sam3s4_gcc_dbg.a 

libchip_sam3s4_gcc_dbg.a:
	@echo --- Making $@
	@$(MAKE) CHIP=sam3s4 DEBUG=1 $(SUBMAKE_OPTIONS) -f sam3.mk

libchip_sam3s4_gcc_rel.a:
	@echo --- Making $@
	@$(MAKE) CHIP=sam3s4 $(SUBMAKE_OPTIONS) -f sam3.mk

.PHONY: clean
clean:
	@echo --- Cleaning sam3s4 release and debug
	@$(MAKE) CHIP=sam3s4 $(SUBMAKE_OPTIONS) -f sam3.mk $@
	@$(MAKE) CHIP=sam3s4 DEBUG=1 $(SUBMAKE_OPTIONS) -f sam3.mk $@


