ifeq ("$(VARIANTS)", "")
VARIANTS = sam3s_ek sam3u_ek arduino_due
#VARIANTS = sam3u_ek
endif

SUBMAKE_OPTIONS=--no-builtin-rules --no-builtin-variables

#-------------------------------------------------------------------------------
# Rules
#-------------------------------------------------------------------------------

all: $(VARIANTS)

.PHONY: sam3s_ek 
sam3s_ek:
	@echo --- Making sam3s_ek
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_sam3s_ek.mk
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_sam3s_ek.mk

.PHONY: sam3u_ek
sam3u_ek:
	@echo --- Making sam3u_ek
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_sam3u_ek.mk
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_sam3u_ek.mk

.PHONY: arduino_due
arduino_due:
	@echo --- Making arduino_due
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_arduino_due.mk
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_arduino_due.mk

.PHONY: clean
clean:
	@echo --- Cleaning sam3s_ek
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_sam3s_ek.mk $@
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_sam3s_ek.mk $@

	@echo --- Cleaning sam3u_ek
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_sam3u_ek.mk $@
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_sam3u_ek.mk $@

	@echo --- Cleaning arduino_due
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_arduino_due.mk $@
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_arduino_due.mk $@


