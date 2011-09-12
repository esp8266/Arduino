# Makefile for compiling libchip

SUBMAKE_OPTIONS=--no-builtin-rules --no-builtin-variables

#-------------------------------------------------------------------------------
# Rules
#-------------------------------------------------------------------------------

# libsam_sam3s4c_gcc_rel.a libsam_sam3u4e_gcc_rel.a
all: libsam_sam3s4c_gcc_dbg.a libsam_sam3u4e_gcc_dbg.a

.PHONY: libsam_sam3s4c_gcc_dbg.a
libsam_sam3s4c_gcc_dbg.a:
	@echo --- 
	@echo --- 
	@echo --- Making $@
	@echo --- 
	@$(MAKE) CHIP=__SAM3S4C__ DEBUG=1 $(SUBMAKE_OPTIONS) -f sam3.mk

.PHONY: libsam_sam3s4c_gcc_rel.a
libsam_sam3s4c_gcc_rel.a:
	@echo --- 
	@echo --- 
	@echo --- Making $@
	@echo --- 
	@$(MAKE) CHIP=__SAM3S4C__ $(SUBMAKE_OPTIONS) -f sam3.mk

.PHONY: libsam_sam3u4e_gcc_dbg.a
libsam_sam3u4e_gcc_dbg.a:
	@echo --- 
	@echo --- 
	@echo --- Making $@
	@echo --- 
	@$(MAKE) CHIP=__SAM3U4E__ DEBUG=1 $(SUBMAKE_OPTIONS) -f sam3.mk

.PHONY: libsam_sam3u4e_gcc_rel.a
libsam_sam3u4e_gcc_rel.a:
	@echo --- 
	@echo --- 
	@echo --- Making $@
	@echo --- 
	@$(MAKE) CHIP=__SAM3U4E__ $(SUBMAKE_OPTIONS) -f sam3.mk

.PHONY: clean
clean:
	@echo --- Cleaning sam3s4c release and debug
	@$(MAKE) CHIP=__SAM3S4C__ $(SUBMAKE_OPTIONS) -f sam3.mk $@
	@$(MAKE) CHIP=__SAM3S4C__ DEBUG=1 $(SUBMAKE_OPTIONS) -f sam3.mk $@

	@echo --- Cleaning sam3u4e release and debug
	@$(MAKE) CHIP=__SAM3U4E__ $(SUBMAKE_OPTIONS) -f sam3.mk $@
	@$(MAKE) CHIP=__SAM3U4E__ DEBUG=1 $(SUBMAKE_OPTIONS) -f sam3.mk $@


