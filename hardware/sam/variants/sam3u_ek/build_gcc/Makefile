# Makefile for compiling libboard
BOARD = 

SUBMAKE_OPTIONS=--no-builtin-rules --no-builtin-variables

#-------------------------------------------------------------------------------
# Rules
#-------------------------------------------------------------------------------

all: sam3u_ek

.PHONY: sam3u_ek
sam3u_ek:
	@echo --- Making sam3u_ek
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libvariant_sam3u_ek.mk
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libvariant_sam3u_ek.mk

.PHONY: clean
clean:
	@echo --- Cleaning sam3u_ek
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libvariant_sam3u_ek.mk $@
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libvariant_sam3u_ek.mk $@


